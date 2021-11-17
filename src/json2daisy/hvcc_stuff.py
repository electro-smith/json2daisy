
def verify_param_exists(name, original_name, components, input=True):
	for comp in components:

		# Dealing with the cvouts the way we have it set up is really annoying
		if comp['component'] == 'CVOuts':
			
			if name == comp['name']:
				if input:
					raise TypeError(f'Parameter "{original_name}" cannot be used as an {"input" if input else "output"}')
				return
		else:
			variants = [mapping['name'].format_map({'name': comp['name']}) for mapping in comp['mapping']]
			if name in variants:
				if input and comp['direction'] == 'output' or not input and comp['direction'] == 'input':
					raise TypeError(f'Parameter "{original_name}" cannot be used as an {"input" if input else "output"}')
				return

	raise NameError(f'Unknown parameter "{original_name}"')

def verify_param_direction(name, components):
	for comp in components:
		if comp['component'] == 'CVOuts':
			if name == comp['name']:
				return True
		else:
			variants = [mapping['name'].format_map({'name': comp['name']}) for mapping in comp['mapping']]
			if name in variants:
				return True

def get_root_component(variant, original_name, components):
	for comp in components:
		if comp['component'] == 'CVOuts':
			if variant == comp['name']:
				return variant
		else:
			variants = [mapping['name'].format_map({'name': comp['name']}) for mapping in comp['mapping']]
			if variant in variants:
				return comp['name']
	raise NameError(f'Unknown parameter "{original_name}"')

def get_component_mapping(component_variant, original_name, component, components):
	for variant in component['mapping']:
		if component['component'] == 'CVOuts':
			stripped = variant['name'].format_map({'name': ''})
			if stripped in component['name']:
				return variant
		elif variant['name'].format_map({'name': component['name']}) == component_variant:
			return variant
	raise NameError(f'Unknown parameter "{original_name}"')

def verify_param_used(component, params_in, params_out, params_in_original_name, params_out_original_name, components):
	# Exclude parents, since they don't have 1-1 i/o mapping
	if component.get('is_parent', False):
		return True
	for param in {**params_in, **params_out}:
		root = get_root_component(param, ({**params_in_original_name, **params_out_original_name})[param], components)
		if root == component['name']:
			return True
	return False

def de_alias(name, aliases, components):
	low = name.lower()
	# simple case
	if low in aliases:
		return aliases[low]
	# aliased variant
	potential_aliases = list(filter(lambda x: x in low, aliases))
	for alias in potential_aliases:
		try:
			target_component = list(filter_match(components, 'name', aliases[alias]))[0]
			# The CVOuts setup really bothers me
			if target_component['component'] != 'CVOuts':
				for mapping in target_component['mapping']:
					if mapping['name'].format_map({'name': alias}) == low:
						return mapping['name'].format_map({'name': aliases[alias]})
		except IndexError:
			# No matching alias from filter
			pass
	# otherwise, it's a direct parameter or unkown one
	return low

def hvcc_ioparse():
  # Verify that the params are valid and remove unused components
  params_in = {}
  params_in_original_names = {}
  for key, item in parameters['in']:
    de_aliased = de_alias(key, target['aliases'], components)
    params_in[de_aliased] = item
    params_in_original_names[de_aliased] = key

  params_out = {}
  params_out_original_names = {}
  for key, item in parameters['out']:
    de_aliased = de_alias(key, target['aliases'], components)
    params_out[de_aliased] = item
    params_out_original_names[de_aliased] = key

  [verify_param_exists(key, params_in_original_names[key], components, input=True) for key in params_in]
  [verify_param_exists(key, params_out_original_names[key], components, input=False) for key in params_out]

  for i in range(len(components) - 1, -1, -1):
    if not verify_param_used(components[i], params_in, params_out, params_in_original_names, params_out_original_names, components):
      components.pop(i)

def hvcc_iowrite():
  out_idx = 0

  for param_name, param in params_in.items():
    root = get_root_component(param_name, params_in_original_names[param_name], components)
    component = list(filter_match(components, 'name', root))[0]
    param_struct = {'hash': param['hash'], 'name': root, 'type': component['component'].upper()}
    replacements['parameters'].append(param_struct)
    mapping = get_component_mapping(param_name, params_in_original_names[param_name], component, components)

    
    write_location = 'callback_write_out' if mapping.get('where', 'callback') == 'callback' else 'loop_write_out'
    component_info = deepcopy(component)
    # A bit of a hack to get cv_1, etc to be written as CV_1
    component_info['name'] = root.upper() if driver == 'patch_sm' and component['component'] == 'AnalogControl' else root
    component_info['value'] = f'output_data[{out_idx}]'
    component_info['default_prefix'] = component.get("default_prefix", '') if component.get('default', False) else ''
    process = mapping["get"].format_map(component_info)

    replacements['callback_write_in'].append({"process": process, "bool": mapping["bool"], "hash": param["hash"]})

  for param_name, param in params_out.items():
    root = get_root_component(param_name, params_out_original_names[param_name], components)
    component = list(filter_match(components, 'name', root))[0]
    param_struct = {'hash': param['hash'], 'index': out_idx, 'name': param_name}
    replacements['output_parameters'].append(param_struct)
    mapping = get_component_mapping(param_name, params_out_original_names[param_name], component, components)

    default_prefix = component.get("default_prefix", '') if component.get('default', False) else ''
    write_location = 'callback_write_out' if mapping.get('where', 'callback') == 'callback' else 'loop_write_out'
    component_info = deepcopy(component)
    component_info['name'] = root
    component_info['value'] = f'output_data[{out_idx}]'
    component_info['default_prefix'] = default_prefix
    write = mapping["set"].format_map(component_info)

    replacements[write_location] += f'\n\t\t{write}'
    out_idx += 1

  replacements['output_comps'] = len(replacements['output_parameters'])
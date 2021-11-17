

def hvcc_stuff():
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
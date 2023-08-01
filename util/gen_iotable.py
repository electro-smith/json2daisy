"""
This utility will automatically generate markup tables describing the I/O for all
included board descriptions.
"""

import json
from os import path, listdir

json_path = path.join(path.dirname(path.abspath(__file__)), '..', 'src', 'json2daisy', 'resources')
non_templates = ['component_defaults', 'component_defaults_patchsm', 'LICENSE']

clarified_components = {
    'AnalogControl': 'Voltage Input',
    'AnalogControlBipolar': 'Bipolar Voltage Input',
    'CD4051AnalogControl': 'Voltage Input',
    'RgbLed': 'RGB LED',
    'Led': 'LED',
    'PCA9685RgbLed': 'RGB LED',
    'PCA9685Led': 'LED',
    'GateOut': 'Gate Out',
    'GateIn': 'Gate In',
    'CVOuts': 'CV Out',
    'Switch3': 'SPDT Switch',
    'CD4021Switch': 'Switch'
}


def gen_row(items):
    return f'| {" | ".join(items)} |\n'


def gen_table(column_headers, rows):
    for row in rows:
        if len(row) > len(column_headers):
            raise OverflowError("A row exceeds the given table width")
        elif len(row) < len(column_headers):
            row += ['---' for i in range(len(column_headers) - len(row))]
        for i in range(len(row)):
            if row[i] == '':
                row[i] = '---'

    table = gen_row(column_headers)
    table += gen_row(['---' for i in range(len(column_headers))])

    for row in rows:
        table += gen_row(row)

    return table


def get_default(som, component):
    defaults_path = path.join(json_path, 'component_defaults.json')
    defaults_sm_path = path.join(json_path, 'component_defaults_patchsm.json')
    if som == 'seed':
        with open(defaults_path, 'rb') as file:
            defaults = json.load(file)
    elif som == 'patch_sm':
        with open(defaults_sm_path, 'rb') as file:
            defaults = json.load(file)
    return defaults[component]


def extract_rows(json_file):
    with open(json_file, 'rb') as file:
        description = json.load(file)

    som = description.get('som', 'seed')

    rows = []
    for compname, comp in description['components'].items():
        temprow = [compname]
        aliases = []
        for alias, root in description['aliases'].items():
            if root == compname:
                aliases.append(alias)
        temprow.append(', '.join(aliases))
        if comp['component'] in clarified_components:
            temprow.append(clarified_components[comp['component']])
        else:
            temprow.append(comp['component'])

        comp_desc = get_default(som, comp['component'])
        variants = []
        for mapping in comp_desc['mapping']:
            var_name = mapping['name'].format_map({'name': compname})
            if var_name != compname and 'cvout' not in var_name:
                variants.append(var_name)
        temprow.append(', '.join(variants))

        rows.append(temprow)

    return rows


if __name__ == '__main__':
    included_json = [item.replace('.json', '') for item in listdir(json_path)]
    headers = ['Name', 'Aliases', 'Type', 'Variants']

    tables = {}
    for description in included_json:
        if description not in non_templates:
            rows = extract_rows(path.join(json_path, description + '.json'))
            temp_table = gen_table(headers, rows)
            tables[description] = temp_table

    tables_path = 'io_tables.md'
    with open(tables_path, 'w') as file:
        for name, table in tables.items():
            file.write(f'## {name}\n\n')
            file.write(table + '\n')

    print(f'Successfully generated table in "{tables_path}"')

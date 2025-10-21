import os
TEXTURE_DIR = '../../../../../../Data/4_Item/1_Armor/1_Male/08_Back/Texture/'
MESH_DIR = '../../../../../../Data/4_Item/1_Armor/1_Male/08_Back/'
FILENAME_TAG = '_CAT_EVENT'
BALLONS = []
MESHES = os.listdir(MESH_DIR)
idx = 1

for texture in os.listdir(TEXTURE_DIR):
    if FILENAME_TAG in texture:
        # print(f'Found ballon: {texture}')
        mesh_name = texture.split(FILENAME_TAG)[0] + '.nif'
        if mesh_name not in MESHES:
            print(f'Not found mesh with name: {mesh_name}')
        xml_template = f'''<?xml version="1.0" encoding="euc-kr"?>
<ITEM NAME="Event_Bak" ATTACH_TO="char_root">
	<MALE>
		<MESH>../Data/4_item/1_Armor/1_Male/08_Back/{mesh_name}</MESH>
		<TEXTURE SRC="100_Bak_3003_0101.dds">../Data/4_item/1_Armor/1_Male/08_Back/Texture/{texture}</TEXTURE>
	</MALE>
	<FEMALE>
		<MESH>../Data/4_item/1_Armor/1_Male/08_Back/{mesh_name}</MESH>
		<TEXTURE SRC="100_Bak_3003_0101.dds">../Data/4_item/1_Armor/1_Male/08_Back/Texture/{texture}</TEXTURE>
	</FEMALE>
</ITEM>'''
        filename_template = f'100_Bak_10{idx:02}.xml'
        with open(filename_template, 'w+') as ff:
            ff.write(xml_template)
        idx = idx + 1


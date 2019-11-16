//TODO

	VP.cpp fifo(785)?????
file:
		
DMA.cpp
		whole file - change name of SV_LEN 
				   - delete unnecessary comments
DMA.hpp
		-- no changes

Format.hpp -delete it if you are not using colors

Image_preprocessor.cpp
		whole file - change name of SV_LEN 
				   - change commented lines (DSKW and similar)
Image_preprocessor.hpp
		-- no changes
		
IntCon.cpp
		-- no changes
IntCon.hpp
		-- no changes

MemCtrl.cpp
		line 16 - parametrize expression in brackets
		line 60 - delete commented line
		lines 65-130 -get a grip of yourself, gal (unnecessary variable definitions, formatting, add comments to the code)
					line 66 - is len[2] array necessary?
		line 167 - parametrize 784
		line 169 -    -||-
MemCtrl.hpp
		line 34 - array num_of_neuros contains num of leurons in l1 and l2, based on counting lines
					perhaps there is no need for neuron_array in Types.hpp?
		line 39 - function sum_of_suv does not exist anymore, delete it

Mlp.cpp
		whole file  - change name of SV_LEN
		line 11     - parametrize 784
		lines 12-13 - parametrize 30 and 10?
		line 97 	- is res_v.clear() necessary? look @ line 155
Mlp.hpp
		line 37 - vector output_v is unnecessary?
		line 38 tmp_res unnecessary
			p is also unnecessary, was used in svm version
			
RAM.cpp
		whole file  - change name of SV_LEN
RAM.hpp
		-- no changes

TB.cpp  whole file - colors & SV_LEN


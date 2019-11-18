#include "Parsers.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <cstring>




//function that splits a string into an std::vector of (strings)
//void split2(std::string to_split, std::string delim, std::vector<std::string>& result) {
//	// Skip delimiters at beginning.
//	std::size_t  lastPos = to_split.find_first_not_of(delim, 0);
//	// Find first "non-delimiter".
//	std::size_t  pos = to_split.find_first_of(delim, lastPos);
//	int delim_num = 0;
//	while (std::string::npos != pos || std::string::npos != lastPos)
//	{
//		// Found a token, add it to the vector.
//		result.push_back(to_split.substr(lastPos, pos - lastPos));
//		delim_num++;
//		// Skip delimiters.  Note the "not_of"
//		lastPos = to_split.find_first_not_of(delim, pos);
//		// Find next "non-delimiter"
//		pos = to_split.find_first_of(delim, lastPos);
//	}
//	std::cout << result[0] << std::endl;
//}
void split(std::string to_split, char delim, std::vector<std::string>& result) {
	std::replace(to_split.begin(), to_split.end(), delim, ' ');
	int delim_num = count(to_split.begin(), to_split.end(), delim);
	std::string frag;
	for (int i = 0; i < to_split.size();i++) {
		if (to_split.at(i) != ' ') {
			frag += to_split.at(i);
			//std::cout << frag << std::endl;
		}
		else if(to_split.at(i) == ' ') {
			delim_num++;
			result.push_back(frag);
			//std::cout << frag <<"\n" << std::endl;
			frag = ' ';
		}
	}
	result.push_back(frag);

}

//parses a wavefront object into passed arrays
bool Parsers::parseOBJ(std::string filename, std::vector<float>& vertices, std::vector<float>& uvs, std::vector<float>& normals, std::vector<unsigned int>& indices) {
	std::vector< lm::vec3>   unique_vertex, unique_normals;
	std::vector< lm::vec2 >  unique_uvs;
	std::vector<std::string> unique_faces, temp_face_index;
	std::string line;
	int tris_faces = 0;
	int quads_faces = 0;
	std::map<std::string, int> map;
	int map_index = 0;
	std::ifstream myfile("data/assets/" + filename);
	//Let’s read this file until the end :
	if (myfile.is_open()) {
		while (getline(myfile, line))
		{
			std::string text;
			std::vector< std::string> data;
			int spaces_no = 0;
			split(line, ' ', data);
			
			std::string first = data[0];
			int slides = data.size()-1;

			//std::cout << "Talls: " << slides << " First: " << first << std::endl;
			//std::cout << line << std::endl;
			
			// read the first word of the line
			if (first == "v") { //Let’s deal with the vertices first 
				lm::vec3 vertex;
				vertex.x = std::stof(data[1]);
				vertex.y = std::stof(data[2]);
				vertex.z = std::stof(data[3]);
				unique_vertex.push_back(vertex);
			}
			else if (first == "vt") { //same thing for the uv's
				lm::vec2 uv;
				uv.x = std::stof(data[1]);
				uv.y = std::stof(data[2]);
				unique_uvs.push_back(uv);
			}
			else if (first == "vn") { //same thing for the normals
				lm::vec3 normal;
				normal.x = std::stof(data[1]);
				normal.y = std::stof(data[2]);
				normal.z = std::stof(data[3]);
				unique_normals.push_back(normal);

			}
			else if (first == "f" && slides == 3) { //we use 3 strings for the faces
				tris_faces = tris_faces + 3;
				std::string fx, fy, fz;
				fx = data[1];
				fy = data[2];
				fz = data[3];
				std::vector<std::string> _face{ fx,fy,fz };
				unique_faces.insert(unique_faces.end(), std::begin(_face), std::end(_face));
			}
			else if (first == "f" && slides == 4) {
				quads_faces = quads_faces + 4;
				std::string fx, fy, fz,fn;
				fx = data[1];
				fy = data[2];
				fz = data[3];
				fn = data[4];
				std::vector<std::string> _face1{fx,fy,fz};
				std::vector<std::string> _face2{fx,fz,fn};
				unique_faces.insert(unique_faces.end(), std::begin(_face1), std::end(_face1));
				unique_faces.insert(unique_faces.end(), std::begin(_face2), std::end(_face2));
			}
				
			
		}
		myfile.close();
	}
	else {
		std::cerr << "Impossible to open the file !\n";
		return 0;
	}
	int face_num = tris_faces + (quads_faces + quads_faces);
	//Processing the data: unique_faces.size()
	for (int i = 0; i < unique_faces.size(); i++) {
		lm::vec3 vertex, normal;
		lm::vec2 uv;
		std::string index = unique_faces.at(i);
		std::vector< std::string> face_data;
		int face, vert, u_v, norlm;
		if (map.count(index) == 0) { //we only add the value if the array index is unique
			std::pair<std::string, int> faceIndex(index, map_index);
			temp_face_index.push_back(index);
			map.insert(faceIndex);
			indices.push_back(map_index);
			map_index++;
			split(index,'/',face_data);
			vert = stoi(face_data[1]);
			u_v = stoi(face_data[2]);
			norlm = stoi(face_data[3]);
			//we get the correct information according to the int indexes
			vertex = unique_vertex[vert - 1];
			uv = unique_uvs[u_v - 1];
			normal = unique_normals[norlm - 1];
			//giving the correct format to the data
			std::vector<float> _vert{ vertex.x ,vertex.y ,vertex.z }, _uvs{ uv.x ,uv.y }, _nrlm{ normal.x ,normal.y ,normal.z };
			vertices.insert(vertices.end(), std::begin(_vert), std::end(_vert));
			uvs.insert(uvs.end(), std::begin(_uvs), std::end(_uvs));
			normals.insert(normals.end(), std::begin(_nrlm), std::end(_nrlm));
		}
		else {  //if the array index is not unique we should also add the value to indices array 
			indices.push_back(map.at(index));
		}
	}
	return true;
}

// load uncompressed RGB targa file into an OpenGL texture
GLint Parsers::parseTexture(std::string filename) {
	std::string str = filename;
	std::string ext = str.substr(str.size() - 4, 4);


	GLuint texture_id;

	if (ext == ".tga" || ext == ".TGA")
	{
		TGAInfo* tgainfo = loadTGA(filename);
		if (tgainfo == NULL) {
			std::cerr << "ERROR: Could not load TGA file" << std::endl;
			return false;
		}

		//generate new openGL texture and bind it (tell openGL we want to do stuff with it)
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id); //we are making a regular 2D texture

												  //screen pixels will almost certainly not be same as texture pixels, so we need to
												  //set some parameters regarding the filter we use to deal with these cases
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//set the mag filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //set the min filter
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4); //use anisotropic filtering

																		  //this is function that actually loads texture data into OpenGL
		glTexImage2D(GL_TEXTURE_2D, //the target type, a 2D texture
			0, //the base level-of-detail in the mipmap
			(tgainfo->bpp == 24 ? GL_RGB : GL_RGBA), //specified the color channels for opengl
			tgainfo->width, //the width of the texture
			tgainfo->height, //the height of the texture
			0, //border - must always be 0
			(tgainfo->bpp == 24 ? GL_BGR : GL_BGRA), //the format of the incoming data
			GL_UNSIGNED_BYTE, //the type of the incoming data
			tgainfo->data); // a pointer to the incoming data

							//we want to use mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);

		//clean up memory
		delete tgainfo->data;
		delete tgainfo;
		return texture_id;
	}
	else {
		std::cerr << "ERROR: No extension or extension not supported" << std::endl;
		return -1;
	}
}

// this reader supports only uncompressed RGB targa files with no colour table
TGAInfo* Parsers::loadTGA(std::string filename)
{
	//the TGA header is 18 bytes long. The first 12 bytes are for specifying the compression
	//and various fields that are very infrequently used, and hence are usually 0.
	//for this limited file parser, we start by reading the first 12 bytes and compare
	//them against the pattern that identifies the file a simple, uncompressed RGB file.
	//more info about the TGA format cane be found at http://www.paulbourke.net/dataformats/tga/

	char TGA_uncompressed[12] = { 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
	char TGA_compare[12];
	char info_header[6];
	GLuint bytes_per_pixel;
	GLuint image_size;

	//open file
	std::ifstream file(filename, std::ios::binary);

	//read first 12 bytes
	file.read(&TGA_compare[0], 12);
	std::streamsize read_header_12 = file.gcount();
	//compare to check that file in uncompressed (or not corrupted)
	int header_compare = memcmp(TGA_uncompressed, TGA_compare, sizeof(TGA_uncompressed));
	if (read_header_12 != sizeof(TGA_compare) || header_compare != 0) {
		std::cerr << "ERROR: TGA file is not in correct format or corrupted: " << filename << std::endl;
		file.close();
		return nullptr;
	}

	//read in next 6 bytes, which contain 'important' bit of header
	file.read(&info_header[0], 6);

	TGAInfo* tgainfo = new TGAInfo;

	tgainfo->width = info_header[1] * 256 + info_header[0]; //width is stored in first two bytes of info_header
	tgainfo->height = info_header[3] * 256 + info_header[2]; //height is stored in next two bytes of info_header

	if (tgainfo->width <= 0 || tgainfo->height <= 0 || (info_header[4] != 24 && info_header[4] != 32)) {
		file.close();
		delete tgainfo;
		std::cerr << "ERROR: TGA file is not 24 or 32 bits, or has no width or height: " << filename << std::endl;
		return NULL;
	}

	//calculate bytes per pixel and then total image size in bytes
	tgainfo->bpp = info_header[4];
	bytes_per_pixel = tgainfo->bpp / 8;
	image_size = tgainfo->width * tgainfo->height * bytes_per_pixel;

	//reserve memory for the image data
	tgainfo->data = (GLubyte*)malloc(image_size);

	//read data into memory
	file.read((char*)tgainfo->data, image_size);
	std::streamsize image_read_size = file.gcount();

	//check it has been read correctly
	if (image_read_size != image_size) {
		if (tgainfo->data != NULL)
			free(tgainfo->data);
		file.close();
		std::cerr << "ERROR: Could not read tga data: " << filename << std::endl;
		delete tgainfo;
		return NULL;
	}

	file.close();

	return tgainfo;
}

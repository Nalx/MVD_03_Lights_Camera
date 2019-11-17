#include "Parsers.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <map>

//function that splits a string into an std::vector of (strings)
void split(std::string to_split, std::string delim, std::vector<std::string>& result) {
	

}

//parses a wavefront object into passed arrays
bool Parsers::parseOBJ(std::string filename, std::vector<float>& vertices, std::vector<float>& uvs, std::vector<float>& normals, std::vector<unsigned int>& indices) {
	std::vector< lm::vec3>   unique_vertex, unique_normals;
	std::vector< lm::vec2 >  unique_uvs;
	std::vector<std::string> unique_faces, temp_face_index;
	std::string line;
	std::map<std::string, int> map;
	int map_index = 0;
	std::ifstream myfile("data/assets/"+filename);
	//Let’s read this file until the end :
	if (myfile.is_open()) {
		while (getline(myfile, line))
		{
			std::string text;
			// read the first word of the line
			myfile >> text;
			if (text == "v") { //Let’s deal with the vertices first 
				lm::vec3 vertex;
				myfile >> vertex.x >> vertex.y >> vertex.z;
				unique_vertex.push_back(vertex);
			}
			else if (text == "vt") { //same thing for the uv's
				lm::vec2 uv;
				myfile >> uv.x >> uv.y;
				unique_uvs.push_back(uv);
			}
			else if (text == "vn") { //same thing for the normals
				lm::vec3 normal;
				myfile >> normal.x >> normal.y >> normal.z;
				unique_normals.push_back(normal);
			}
			else if (text == "f") { //we use 3 strings for the faces 
				std::string fx, fy, fz;
				myfile >> fx >> fy >> fz;
				std::vector<std::string> _face{fx,fy,fz};
				unique_faces.insert(unique_faces.end(), std::begin(_face), std::end(_face));
			}
		}
		myfile.close();
	}
	else {
		std::cerr << "Impossible to open the file !\n";
		return 0;
	}
	//Processing the data:
	for (int i = 0; i < unique_faces.size(); i++) {
		lm::vec3 vertex, normal;
		lm::vec2 uv;
		std::string index = unique_faces.at(i);
		int face, vert, u_v, norlm;
		if (map.count(index) == 0) { //we only add the value if the array index is unique
			std::pair<std::string, int> faceIndex(index, map_index);
			temp_face_index.push_back(index);
			map.insert(faceIndex); 
			indices.push_back(map_index);
			map_index++;
			index.erase(std::remove(index.begin(), index.end(), '/'), index.end()); //get rid of "/"
		    face = atoi(index.c_str()); //string to integer
			//we've to get each value as separate int
		    vert = floor((face / 100) % 10);
		    u_v = floor((face / 10) % 10);
		    norlm = floor((face % 10));
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

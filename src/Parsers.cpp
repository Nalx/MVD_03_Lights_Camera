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

    //obj parser goes here

	std::vector< GLfloat > position_buffer_data, texture_buffer_data, normal_buffer_data;
	GLuint vertexIndex_data;
	GLuint uvIndex_data;
	GLuint normalIndex_data;

	std::vector< lm::vec3>  unique_vertex;
	std::vector< lm::vec2 >  unique_uvs;
	std::vector< lm::vec3 >  unique_normals;

	std::vector<std::string> temp_face_index;
	std::vector<float> out_vertices;
	std::vector<float> out_uvs;
	std::vector<float> out_normals;
	std::string line;
	std::map<std::string, int> umap;
	int map_index = 0;

	std::ifstream myfile("data/assets/"+filename);
	
	if (myfile.is_open()) {
		while (getline(myfile, line))
		{

			std::string text;
			myfile >> text;

			//std::cerr << text << std::endl;
			if (text == "v") {
				lm::vec3 vertex;
				myfile >> vertex.x;
				myfile >> vertex.y;
				myfile >> vertex.z;
				


				unique_vertex.push_back(vertex);
			std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
			}
			else if (text == "vt") {
				lm::vec2 uv;
				myfile >> uv.x;
				myfile >> uv.y;

				unique_uvs.push_back(uv);
				std::cout << uv.x << " " << uv.y << " " << std::endl;
			}
			else if (text == "vn") {
				lm::vec3 normal;
				myfile >> normal.x;
				myfile >> normal.y;
				myfile >> normal.z;

				unique_normals.push_back(normal);
				std::cout << normal.x << " " << normal.y << " " << normal.z << std::endl;
			}
			else if (text == "f") {
				std::string fx, fy, fz;
				GLuint  vertexIndex[3], uvIndex[3], normalIndex[3];
				lm::vec3 vertex;
				lm::vec2 uv;
				lm::vec3 normal;
				myfile >> fx >> fy >> fz;
				std::string result;
				//Si el fx no existeix dins el umap creo un registre del valor amb un index a una posició més
				//Si el fx existeix genero una posició més a l'array de index finals de valor int del umap i passo d'afegirlo al umap
				if (umap.count(fx)==0){
					std::cout << "UMAP INDEX: " << fx << std::endl;
					std::pair<std::string, int> faceIndex(fx, map_index);
					temp_face_index.push_back(fx);
					umap.insert(faceIndex);
					indices.push_back(map_index);
					fx.erase(std::remove(fx.begin(), fx.end(), '/'), fx.end());
					int face = atoi(fx.c_str());
					int vert = floor((face / 100) % 10);
					int u_v = floor((face / 10) % 10);
					int norlm = floor((face % 10));
					vertex= unique_vertex[vert - 1];
					uv= unique_uvs[u_v - 1];
					normal= unique_normals[norlm - 1];
					out_vertices.push_back(vertex.x);
					out_vertices.push_back(vertex.y);
					out_vertices.push_back(vertex.z);
					out_uvs.push_back(uv.x);
					out_uvs.push_back(uv.y);
					out_normals.push_back(normal.x);
					out_normals.push_back(normal.y);
					out_normals.push_back(normal.z);
					map_index++;
				}
				else {
					indices.push_back(umap.at(fx));
				}
				if (umap.count(fy) == 0) {
					temp_face_index.push_back(fy);
					std::cout << "UMAP INDEX: " << fy << std::endl;
					std::pair<std::string, int> faceIndex(fy, map_index);
					umap.insert(faceIndex);
					indices.push_back(map_index);
					fy.erase(std::remove(fy.begin(), fy.end(), '/'), fy.end());
					int face = atoi(fy.c_str());
					int vert = floor((face / 100) % 10);
					int u_v = floor((face / 10) % 10);
					int norlm = floor((face % 10));
					vertex = unique_vertex[vert - 1];
					uv = unique_uvs[u_v - 1];
					normal = unique_normals[norlm - 1];
					out_vertices.push_back(vertex.x);
					out_vertices.push_back(vertex.y);
					out_vertices.push_back(vertex.z);
					out_uvs.push_back(uv.x);
					out_uvs.push_back(uv.y);
					out_normals.push_back(normal.x);
					out_normals.push_back(normal.y);
					out_normals.push_back(normal.z);
					map_index++;
				}
				else {
					indices.push_back(umap.at(fy));
				}
				if (umap.count(fz) == 0) {
					temp_face_index.push_back(fz);
					std::cout << "UMAP INDEX: " << fz << std::endl;
					std::pair<std::string, int> faceIndex(fz, map_index);
					umap.insert(faceIndex);
					indices.push_back(map_index);
					fz.erase(std::remove(fz.begin(), fz.end(), '/'), fz.end());
					int face = atoi(fz.c_str());
					int vert = floor((face / 100) % 10);
					int u_v = floor((face / 10) % 10);
					int norlm = floor((face % 10));
					vertex = unique_vertex[vert - 1];
					uv = unique_uvs[u_v - 1];
					normal = unique_normals[norlm - 1];
					out_vertices.push_back(vertex.x);
					out_vertices.push_back(vertex.y);
					out_vertices.push_back(vertex.z);
					out_uvs.push_back(uv.x);
					out_uvs.push_back(uv.y);
					out_normals.push_back(normal.x);
					out_normals.push_back(normal.y);
					out_normals.push_back(normal.z);
					map_index++;
				}
				else {
					indices.push_back(umap.at(fz));	
				}
			}
		}
		myfile.close();
	}
	else {
		std::cerr << "Impossible to open the file !\n";
		return 0;
	}
	//index buffer
	
	vertices = out_vertices;
	uvs = out_uvs;
	normals = out_normals;
	//We export a .txt file with all the obj data parsed:
	std::ofstream file;
	file.open("OBJ_DEBUG.txt");
	for (unsigned int i = 0; i < vertices.size()-2; i++) {
		std::cout << "VERTEX " << i << ":  " << vertices[i]  << std::endl;
		std::string text = "VERTEX :" + std::to_string(vertices[i]);
			file << text  << '\n';
	}
	for (unsigned int i = 0; i < uvs.size(); i++) {
		std::cout << "UVs " << i << ":" << uvs[i] << std::endl;
		std::string text = "UVs :" +std::to_string((int)uvs[i]);
		file << text << '\n';
	}
	for (unsigned int i = 0; i < normals.size(); i++) {
		std::cout << "Normals " << i << ":" << normals[i] << std::endl;
		std::string text = "Normals :" + std::to_string(normals[i]);
		file << text << '\n';
	}
	for (unsigned int i = 0; i < indices.size(); i++) {
		std::cout << "Indices " << i << ":" << indices[i] << std::endl;
		std::string text = "Indices :" + std::to_string(indices[i]);
		file << text << '\n';
	}
	"Writing this to a file.\n";
	myfile.close();
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

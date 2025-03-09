

/**************
 Creae the meshes, as described below
 
 WARNING!
 Since it is a C program, you can use for loops and functions if you think they can be helpful in your solution.
 However, please include all your code in this file, since it will be put in an automatic correction process
 for the final evaluation. Please also be cautious when using standard libraries and symbols, since they
 might not be available in all the development environments (especially, they might not be available
 in the final evaluation environment, preventing your code from compiling).
 This WARNING will be valid far ALL THE ASSIGNMENTs, but it will not be repeated in the following texts,
 so please remember these advices carefully!
 
***************/

void MakeSquare(float size, std::vector<glm::vec3> &vertices, std::vector<uint32_t> &indices) {
// Creates a square, on the xz-plane, aligned with the axis, and centered in the origin.
// The length of the four sides is in parameter >size<.
// The procedure should fill the array contained in the >vertices< parameter, with the positions of
// the vertices of the primitive, expressed with their local coordinates in a glm::vec3 element. 
// Indices should be returned in the >indices< array, starting from 0, and up to vertices.size()-1.
// The primitive is encoded as an indexed triangle list, so the size of the >indices< array, should
// be a multiple of 3: each group of three indices, defines a different triangle.
//
// HINT: this procedure has already been implemented. You can keep it as is
	vertices = {
				   {-size/2.0f,0.0f,-size/2.0f},
				   {-size/2.0f,0.0f, size/2.0f},
				   { size/2.0f,0.0f,-size/2.0f},
				   { size/2.0f,0.0f, size/2.0f}};
	indices = {0, 1, 2,    1, 3, 2};
}

void MakeCube(float size, std::vector<glm::vec3> &vertices, std::vector<uint32_t> &indices) {
// Creates a cube, with the faces perpendicular to the axis, and centered in the origin.
// The length of one edge of the cube is >size<.
// The procedure should fill the array contained in the >vertices< parameter, with the positions of
// the vertices of the primitive, expressed with their local coordinates in a glm::vec3 element. 
// Indices should be returned in the >indices< array, starting from 0, and up to vertices.size()-1.
// The primitive is encoded as an indexed triangle list, so the size of the >indices< array, should
// be a multiple of 3: each group of three indices, defines a different triangle.
//
// HINT: the procedure below creates a square. You can use it as a side of the cube (please remember
// to change the value of the y component, otherwise the result will be wrong
	//vertices = {
	//			   {-size/2.0f,0.0f,-size/2.0f},
	//			   {-size/2.0f,0.0f, size/2.0f},
	//			   { size/2.0f,0.0f,-size/2.0f},
	//			   { size/2.0f,0.0f, size/2.0f}};
	//indices = {0, 1, 2,    1, 3, 2};
	float a = size / 2.0f;
	vertices = {
				   {a,a,a},{a,a,-a},{-a,a,-a},{-a,a,a},
				   {a,-a,a},{a,-a,-a},{-a,-a,-a},{-a,-a,a},
	};
	indices = { 3, 4, 0,   3, 7, 4 ,   0,1,3,   1,2,3,  1,0,4,  1,4,5,  7,4,5,  6,7,5, 1,6,5,   1,2,6,  2,7,3,  2,6,7};

}

void MakeCylinder(float radius, float height, int slices, std::vector<glm::vec3> &vertices, std::vector<uint32_t> &indices) {
// Creates a cylinder, approximated by a prism with a base composed by a regular polygon with >slices< sides.
// The radius of the cylinder is >radius<, and it height is >height<. The cylinder has its centere
// in the origin, and spans half above and half below the plane that passes thorugh the origin.
// The top and bottom caps are are aligned with xz-plane and perpendicular to the y-axis.
// The procedure should fill the array contained in the >vertices< parameter, with the positions of
// the vertices of the primitive, expressed with their local coordinates in a glm::vec3 element. 
// Indices should be returned in the >indices< array, starting from 0, and up to vertices.size()-1.
// The primitive is encoded as an indexed triangle list, so the size of the >indices< array, should
// be a multiple of 3: each group of three indices, defines a different triangle.
//
// HINT: the procedure below creates a rectangle. You have to change it, or you will obtain a wrong result
// You should use a for loop, and you should start from the procedure to create a circle seen during the lesson
	vertices.resize(2*(slices+1));
	indices.resize(12 * slices);
	for (int i = 0; i < slices; i++)
	{
		float ang = 2 * 3.1415926 * (float)i / slices;
		vertices[i] = glm::vec3(radius * sin(ang), height / 2.0f, radius * cos(ang));
	}
	vertices[slices] = glm::vec3(0.0f, height/2.0f, 0.0f);
	for (int i = 0; i < slices; i++)
	{
		indices[3 * i] = slices;
		indices[3 * i + 1] = i;
		indices[3 * i + 2] = (i+1)% slices;
	}
	for (int i = slices+1; i < 2*slices+1; i++)
	{
		float ang = 2 * 3.1415926 * (float)(i-slices-1) / slices;
		vertices[i] = glm::vec3(radius * sin(ang), -height / 2.0f, radius * cos(ang));
	}
	vertices[2*(slices)+1] = glm::vec3(0.0f, -height / 2.0f, 0.0f);
	for (int i = slices; i < 2*slices; i++)
	{
		indices[3 * i] = 2*(slices)+1;
		indices[3 * i + 1] = i;
		if(3*i+2==6* slices-1)
		indices[3 * i + 2] = slices+1;
		else
			indices[3 * i + 2] = (i + 1);
	}
	for (int i = slices; i < 2 * slices-1; i++)
	{

		indices[6 * i] = i - slices;
		indices[6 * i + 1] = i + 1;
		indices[6 * i + 2] = i + 2;
		indices[6 * i + 3] = i - slices;
		indices[6 * i + 4] = i + 2;
		indices[6 * i + 5] = i - slices + 1;
	}
	indices[12 * slices - 6] = slices+1;
	indices[12 * slices - 5] = slices -1;
	indices[12 * slices - 4] = 2*slices;
	indices[12 * slices - 3] = slices+1 ;
	indices[12 * slices - 2] = 0;
	indices[12 * slices - 1] = slices-1;
}

void MakeCone(float radius, float height, int slices, std::vector<glm::vec3> &vertices, std::vector<uint32_t> &indices) {
// Creates a cone, approximated by a pyramid with a base composed by a regular polygon with >slices< sides.
// The radius of the cone is >radius<, and it height is >height<. The cone has its centere
// in the origin, and spans half above and half below the plane that passes thorugh the origin.
// The bottom cap is aligned with xz-plane and perpendicular to the y-axis.
// The procedure should fill the array contained in the >vertices< parameter, with the positions of
// the vertices of the primitive, expressed with their local coordinates in a glm::vec3 element. 
// Indices should be returned in the >indices< array, starting from 0, and up to vertices.size()-1.
// The primitive is encoded as an indexed triangle list, so the size of the >indices< array, should
// be a multiple of 3: each group of three indices, defines a different triangle.
//
// HINT: the procedure below creates a triangle. You have to change it, or you will obtain a wrong result
// You should use a for loop, and you should start from the procedure to create a circle seen during the lesson
	vertices.resize(2 * (slices + 1));
	indices.resize(6 * slices);
	for (int i = 0; i < slices; i++)
	{
		float ang = 2 * 3.1415926 * (float)i / slices;
		vertices[i] = glm::vec3(radius * sin(ang), -height / 2.0f, radius * cos(ang));
	}
	vertices[slices] = glm::vec3(0.0f, -height / 2.0f, 0.0f);
	for (int i = 0; i < slices; i++)
	{
		indices[3 * i] = slices;
		indices[3 * i + 1] = i;
		indices[3 * i + 2] = (i + 1) % slices;
	}
	for (int i = slices + 1; i < 2 * slices + 1; i++)
	{
		float ang = 2 * 3.1415926 * (float)(i - slices - 1) / slices;
		vertices[i] = glm::vec3(radius * sin(ang), -height / 2.0f, radius * cos(ang));
	}
	vertices[2 * (slices)+1] = glm::vec3(0.0f, height / 2.0f, 0.0f);
	for (int i = slices; i < 2 * slices; i++)
	{
		indices[3 * i] = 2 * (slices)+1;
		indices[3 * i + 1] = i;
		if (3 * i + 2 == 6 * slices - 1)
			indices[3 * i + 2] = slices + 1;
		else
			indices[3 * i + 2] = (i + 1);
	}
}

void MakeSphere(float radius, int rings, int slices, std::vector<glm::vec3> &vertices, std::vector<uint32_t> &indices)
{
// Creates a sphere, approximated by a poliedron composed by >slices<, and >rings< rings.
// The radius of the sphere is >radius<, and it is centered in the origin.
// The procedure should fill the array contained in the >vertices< parameter, with the positions of
// the vertices of the primitive, expressed with their local coordinates in a glm::vec3 element. 
// Indices should be returned in the >indices< array, starting from 0, and up to vertices.size()-1.
// The primitive is encoded as an indexed triangle list, so the size of the >indices< array, should
// be a multiple of 3: each group of three indices, defines a different triangle.
//
// HINT: the procedure below creates a circle. You have to change it, or you will obtain a wrong result
// You should use two nested for loops, one used to span across the rings, and the other that spans along
// the rings.
	vertices.resize((rings+1)*(slices+1));
	indices.resize(6*(rings) * (slices));
	for (int i = 0; i < rings+1; i++)
	{
		float ang1 = 3.1415926 * (float)i / rings;
		for (int j = 0; j < slices+1; j++)
		{
			float ang2 = 2 * 3.1415926 * (float)j / slices;
			vertices[i] = glm::vec3(radius  *sin(ang1)*sin(ang2), radius* cos(ang1), radius * cos(ang1) * sin(ang2));
		}
	}
	for(int j=0;j< slices;j++)
		for (int i = 0; i < rings; i++)
		{
			indices[6 * i] = j * rings + i;
			indices[6 * i+1] = (j + 1) * rings + i + 1;
			indices[6 * i + 2] = (j+1) * rings + i + 2;
			indices[6 * i+3] = j * rings + i;
			indices[6 * i + 1] = j * rings + i + 1;
			indices[6 * i + 2] = (j + 1) * rings + i + 2;
	}
}

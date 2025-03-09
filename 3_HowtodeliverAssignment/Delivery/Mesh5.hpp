

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
       
        
        vertices.clear();
        indices.clear();

        //make a cneter size to get the origin of cube
        float o = size/ 2.0f;
        vertices = {
            {-o,-o,-o},
                {o,-o,-o},
                {o,o,-o},
                    {-o,o,-o},
                        {-o,-o,o},
                            {o,-o,o},
                                {o,o,o},
                                {-o,o,o}
        };
        indices = {
            // Front face (original was 0, 1, 2, 0, 2, 3)
             0, 2, 1, 0, 3, 2,
             // Back face (original was 5, 4, 7, 5, 7, 6)
             5, 7, 4, 5, 6, 7,
             // Top face (original was 3, 2, 6, 3, 6, 7)
             3, 6, 2, 3, 7, 6,
             // Bottom face (original was 4, 5, 1, 4, 1, 0)
             4, 1, 5, 4, 0, 1,
             // Right face (original was 1, 5, 6, 1, 6, 2)
             1, 6, 5, 1, 2, 6,
             // Left face (original was 4, 0, 3, 4, 3, 7)
             4, 3, 0, 4, 7, 3
        };
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
    vertices.clear();
    indices.clear();
    
    float hF = height / 2.0f;
    float aS = 2.0f * glm::pi<float>() / slices;
    
    for (int i = 0; i < slices; ++i) {
            float angle = i * aS;
            float x = radius * cos(angle);
            float z = radius * sin(angle);

            // Bottom vertex
            vertices.push_back(glm::vec3(x, -hF, z));
            // Top vertex
            vertices.push_back(glm::vec3(x, hF, z));
        }

        // ! indices for the side faces
        for (int i = 0; i < slices; ++i) {
            int next = (i + 1) % slices;
            int bottom1 = 2 * i;
            int top1 = 2 * i + 1;
            int bottom2 = 2 * next;
            int top2 = 2 * next + 1;

            // 2 triangles per slice (side of the cylinder)
            indices.push_back(bottom1);
            indices.push_back(top1);
            indices.push_back(top2);

            indices.push_back(bottom1);
            indices.push_back(top2);
            indices.push_back(bottom2);
        }

    // indices!
    vertices.push_back(glm::vec3(0, -hF, 0)); // Center bottom vertex
    vertices.push_back(glm::vec3(0, hF, 0)); // Center top vertex

    int centerBottom = vertices.size() - 2;
    int centerTop = vertices.size() - 1;

    for (int i = 0; i < slices; ++i) {
        int next = (i + 1) % slices;
        int current = 2 * i;
        int nextVert = 2 * next;

        // Bottom cap
        indices.push_back(centerBottom);
        indices.push_back(current);
        indices.push_back(nextVert);

        // Top cap
        indices.push_back(centerTop);
        indices.push_back(nextVert + 1);
        indices.push_back(current + 1);
    }


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
    vertices.clear();
    indices.clear();

     float halfHeight = height / 2.0f;
     float angleStep = 2.0f * glm::pi<float>() / slices;

     // vertices for the cone's base
     for (int i = 0; i < slices; ++i) {
         float angle = i * angleStep;
         float x = radius * cos(angle);
         float z = radius * sin(angle);

         // bottom vertex
         vertices.push_back(glm::vec3(x, -halfHeight, z));
     }

     // top vertex of the cone
     vertices.push_back(glm::vec3(0.0f, halfHeight, 0.0f));
     int topIndex = slices; // Index of the top vertex

     // indices for the cone's side faces
     for (int i = 0; i < slices; ++i) {
         int next = (i + 1) % slices;

         indices.push_back(i);
         indices.push_back(topIndex);
         indices.push_back(next);
     }

     // Indices
     int centerBottom = vertices.size();
     vertices.push_back(glm::vec3(0.0f, -halfHeight, 0.0f)); 
    // Center bottom vertex
     for (int i = 0; i < slices; ++i) {
         int next = (i + 1) % slices;

         indices.push_back(centerBottom);
         indices.push_back(i);
         indices.push_back(next);
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
    vertices.clear();
    indices.clear();

    // Create vertices
    for (int r = 0; r <= rings; r++) {
        float phi = M_PI * float(r) / float(rings);  // Vertical angle
        float y = cos(phi);  // y is calculated the same way as the original circle
        float xz_radius = sin(phi);  // Radius of the ring at height y

        for (int s = 0; s <= slices; s++) {
            float theta = 2 * M_PI * float(s) / float(slices);  // Horizontal angle
            float x = xz_radius * cos(theta);
            float z = xz_radius * sin(theta);
            vertices.push_back(glm::vec3(radius * x, radius * y, radius * z));
        }
    }
    //indices
    for (int r = 0; r < rings; r++) {
        for (int s = 0; s < slices; s++) {
            int cur = r * (slices + 1) + s;
            int next = cur + slices + 1;

            // Ensure the indices are added in a clockwise direction
            indices.push_back(cur + 1);
            indices.push_back(next);
            indices.push_back(cur);

            indices.push_back(cur + 1);
            indices.push_back(next + 1);
            indices.push_back(next);
        }
    }
}

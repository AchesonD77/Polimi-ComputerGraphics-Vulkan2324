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

void MakeSquare(float size, std::vector<std::array<float,6>> &vertices, std::vector<uint32_t> &indices) {
// Creates a square, on the xz-plane, aligned with the axis, and centered in the origin.
// The length of the four sides is in parameter >size<.
// The procedure should fill the array contained in the >vertices< parameter, with the positions of
// the vertices of the primitive, expressed with their local coordinates in a std::array<float,6> element.
// In particular, the first three elements (index 0,1,2) encode the position of the vertex (x in index 0,
// y in index 1, and z in index 2). The second set of three elements (index 3,4,5) encode the direction
// of the normal vector for the considerd vertex (N.x in index 3, N.y in index 4, and N.z in index 5).
// Indices should be returned in the >indices< array, starting from 0, and up to vertices.size()-1.
// The primitive is encoded as an indexed triangle list, so the size of the >indices< array, should
// be a multiple of 3: each group of three indices, defines a different triangle.
//
// HINT: this procedure has already been implemented. You can keep it as is
    vertices = {
                   {-size/2.0f,0.0f,-size/2.0f,0.0f,1.0f,0.0f},
                   {-size/2.0f,0.0f, size/2.0f,0.0f,1.0f,0.0f},
                   { size/2.0f,0.0f,-size/2.0f,0.0f,1.0f,0.0f},
                   { size/2.0f,0.0f, size/2.0f,0.0f,1.0f,0.0f}};
    indices = {0, 1, 2,    1, 3, 2};
}

void MakeCube(float size, std::vector<std::array<float,6>> &vertices, std::vector<uint32_t> &indices) {
// Creates a cube, with the faces perpendicular to the axis, and centered in the origin.
// The length of one edge of the cube is >size<.
// The procedure should fill the array contained in the >vertices< parameter, with the positions of
// the vertices of the primitive, expressed with their local coordinates in a std::array<float,6> element.
// In particular, the first three elements (index 0,1,2) encode the position of the vertex (x in index 0,
// y in index 1, and z in index 2). The second set of three elements (index 3,4,5) encode the direction
// of the normal vector for the considerd vertex (N.x in index 3, N.y in index 4, and N.z in index 5).
// Indices should be returned in the >indices< array, starting from 0, and up to vertices.size()-1.
// The primitive is encoded as an indexed triangle list, so the size of the >indices< array, should
// be a multiple of 3: each group of three indices, defines a different triangle.
//
// HINT: the procedure below creates a square. You can use it as a side of the cube (please remember
// to change the value of the y component, otherwise the result will be wrong
    float half = size / 2.0f;
        vertices.clear();
        indices.clear();

        // vertices for each face of the cube.
        vertices = {
            // front face
            {-half, -half, half,  0.0, 0.0, 1.0},
            { half, -half, half,  0.0, 0.0, 1.0},
            { half,  half, half,  0.0, 0.0, 1.0},
            {-half,  half, half,  0.0, 0.0, 1.0},
            // back face
            { half, -half, -half,  0.0, 0.0, -1.0},
            {-half, -half, -half,  0.0, 0.0, -1.0},
            {-half,  half, -half,  0.0, 0.0, -1.0},
            { half,  half, -half,  0.0, 0.0, -1.0},
            // top face
            {-half, half,  half,  0.0, 1.0, 0.0},
            { half, half,  half,  0.0, 1.0, 0.0},
            { half, half, -half,  0.0, 1.0, 0.0},
            {-half, half, -half,  0.0, 1.0, 0.0},
            // bottom face
            {-half, -half, -half,  0.0, -1.0, 0.0},
            { half, -half, -half,  0.0, -1.0, 0.0},
            { half, -half,  half,  0.0, -1.0, 0.0},
            {-half, -half,  half,  0.0, -1.0, 0.0},
            // right face
            {half, -half,  half,  1.0, 0.0, 0.0},
            {half, -half, -half,  1.0, 0.0, 0.0},
            {half,  half, -half,  1.0, 0.0, 0.0},
            {half,  half,  half,  1.0, 0.0, 0.0},
            // left face
            {-half, -half, -half,  -1.0, 0.0, 0.0},
            {-half, -half,  half,  -1.0, 0.0, 0.0},
            {-half,  half,  half,  -1.0, 0.0, 0.0},
            {-half,  half, -half,  -1.0, 0.0, 0.0},
        };

        // define indices for each face of the cube (two triangles per face)
        indices = {
            // Front face
            0, 1, 2, 0, 2, 3,
            // Back face
            4, 5, 6, 4, 6, 7,
            // Top face
            8, 9, 10, 8, 10, 11,
            // Bottom face
            12, 13, 14, 12, 14, 15,
            // Right face
            16, 17, 18, 16, 18, 19,
            // Left face
            20, 21, 22, 20, 22, 23,
        };
}

void MakeCylinder(float radius, float height, int slices, std::vector<std::array<float,6>> &vertices, std::vector<uint32_t> &indices) {
    // Creates a cylinder, approximated by a prism with a base composed by a regular polygon with >slices< sides.
    // The radius of the cylinder is >radius<, and it height is >height<. The cylinder has its centere
    // in the origin, and spans half above and half below the plane that passes thorugh the origin.
    // The top and bottom caps are are aligned with xz-plane and perpendicular to the y-axis.
    // The procedure should fill the array contained in the >vertices< parameter, with the positions of
    // the vertices of the primitive, expressed with their local coordinates in a std::array<float,6> element.
    // In particular, the first three elements (index 0,1,2) encode the position of the vertex (x in index 0,
    // y in index 1, and z in index 2). The second set of three elements (index 3,4,5) encode the direction
    // of the normal vector for the considerd vertex (N.x in index 3, N.y in index 4, and N.z in index 5).
    // Indices should be returned in the >indices< array, starting from 0, and up to vertices.size()-1.
    // The primitive is encoded as an indexed triangle list, so the size of the >indices< array, should
    // be a multiple of 3: each group of three indices, defines a different triangle.
    //
    // HINT: the procedure below creates a rectangle. You have to change it, or you will obtain a wrong result
    // You should use a for loop, and you should start from the procedure to create a circle seen during the lesson
    vertices.clear();
    indices.clear();

    float halfHeight = height / 2.0f;

    // vertices for the top and bottom circles
    for (int i = 0; i < slices; ++i) {
        float angle = 2 * M_PI * i / slices;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // bottom vertices for the caps and sides
        vertices.push_back({x, -halfHeight, z, 0, -1, 0});  // fr normal
        vertices.push_back({x, halfHeight, z, 0, 1, 0});  // for normal
        // side normals
        float nx = cos(angle);
        float nz = sin(angle);
        vertices.push_back({x, -halfHeight, z, nx, 0, nz}); // Bottom side vertex normal
        vertices.push_back({x, halfHeight, z, nx, 0, nz});  // Top side vertex normal
    }

    int topCenterIndex = vertices.size();
    vertices.push_back({0, halfHeight, 0, 0, 1, 0}); // Top center vertex

    int bottomCenterIndex = vertices.size();
    vertices.push_back({0, -halfHeight, 0, 0, -1, 0}); // Bottom center vertex

    // Indices (top bottom)
    for (int i = 0; i < slices; ++i) {
        int next = (i + 1) % slices;

        // Bottom
        indices.push_back(bottomCenterIndex);
        indices.push_back(i * 4 + 2);
        indices.push_back(next * 4 + 2);
        // Top
        indices.push_back(topCenterIndex);
        indices.push_back(next * 4 + 3);
        indices.push_back(i * 4 + 3);
    }

    // Indices for the side faces
    for (int i = 0; i < slices; ++i) {
        int next = (i + 1) % slices;
        
        indices.push_back(i * 4 + 2);
        indices.push_back(i * 4 + 3);
        indices.push_back(next * 4 + 2);

        indices.push_back(next * 4 + 2);
        indices.push_back(i * 4 + 3);
        indices.push_back(next * 4 + 3);
    }
}


void MakeCone(float radius, float height, int slices, std::vector<std::array<float,6>> &vertices, std::vector<uint32_t> &indices) {
// Creates a cone, approximated by a pyramid with a base composed by a regular polygon with >slices< sides.
// The radius of the cone is >radius<, and it height is >height<. The cone has its centere
// in the origin, and spans half above and half below the plane that passes thorugh the origin.
// The bottom cap is aligned with xz-plane and perpendicular to the y-axis.
// The procedure should fill the array contained in the >vertices< parameter, with the positions of
// the vertices of the primitive, expressed with their local coordinates in a std::array<float,6> element.
// In particular, the first three elements (index 0,1,2) encode the position of the vertex (x in index 0,
// y in index 1, and z in index 2). The second set of three elements (index 3,4,5) encode the direction
// of the normal vector for the considerd vertex (N.x in index 3, N.y in index 4, and N.z in index 5).
// Indices should be returned in the >indices< array, starting from 0, and up to vertices.size()-1.
// The primitive is encoded as an indexed triangle list, so the size of the >indices< array, should
// be a multiple of 3: each group of three indices, defines a different triangle.
//
// HINT: the procedure below creates a triangle. You have to change it, or you will obtain a wrong result
// You should use a for loop, and you should start from the procedure to create a circle seen during the lesson
    vertices.clear();
       indices.clear();

       float halfHeight = height / 2.0f;
       float normY = radius / sqrt(radius * radius + height * height); // normal for sides
       float normXZ = height / sqrt(radius * radius + height * height); // normal for sides

       // vertices
       for (int i = 0; i < slices; ++i) {
           float angle = 2 * M_PI * i / slices;
           float x = radius * cos(angle);
           float z = radius * sin(angle);

           // bottom vertices
           vertices.push_back({x, -halfHeight, z, 0, 1, 0});

           // normal for sides
           float normX = -cos(angle) * normXZ;
           float normZ = -sin(angle) * normXZ;

           // side vertices, base vertex (same location but different normals)
           vertices.push_back({x, -halfHeight, z, normX, normY, normZ});
       }

       // Top vertex
       vertices.push_back({0, halfHeight, 0, 0, -1, 0});

       // bottom
       int bottomCenterIndex = vertices.size();
       vertices.push_back({0, -halfHeight, 0, 0, 1, 0});

       // bottom cap
       for (int i = 0; i < slices; ++i) {
           indices.push_back(bottomCenterIndex);
           indices.push_back(2 * i);
           indices.push_back(2 * ((i + 1) % slices));
       }

       // Create indices for the cone sides
       int apexIndex = vertices.size() - 2;
       for (int i = 0; i < slices; ++i) {
           indices.push_back(apexIndex);
           indices.push_back(2 * ((i + 1) % slices) + 1);
           indices.push_back(2 * i + 1);
       }
}



void MakeSphere(float radius, int rings, int slices, std::vector<std::array<float, 6>> &vertices, std::vector<uint32_t> &indices) {
    vertices.clear();
    indices.clear();

    // Reserve
    vertices.reserve((rings + 1) * (slices + 1));
    indices.reserve(rings * slices * 6);

    // vertices and normals
    for (int ring = 0; ring <= rings; ++ring) {
        float v = (float)ring / (float)rings;
        float phi = v * M_PI;

        for (int slice = 0; slice <= slices; ++slice) {
            float u = (float)slice / (float)slices;
            float theta = u * 2.0f * M_PI;

            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);

            float nx = sin(phi) * cos(theta);
            float ny = cos(phi);
            float nz = sin(phi) * sin(theta);

            vertices.push_back({x, y, z, nx, ny, nz});
        }
    }

    // indices for triangle faces
    for (int ring = 0; ring < rings; ++ring) {
        for (int slice = 0; slice < slices; ++slice) {
            int first = ring * (slices + 1) + slice;
            int second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(first + 1);
            indices.push_back(second);

            indices.push_back(first + 1);
            indices.push_back(second + 1);
            indices.push_back(second);
        }
    }
}


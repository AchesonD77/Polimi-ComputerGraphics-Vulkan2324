

/**************
 Compute the world view and projection matrices, as described below
 
 WARNING!
 Since it is a C program, you can use for loops and functions if you think they can be helpful in your solution.
 However, please include all your code in this file, since it will be put in an automatic correction process
 for the final evaluation. Please also be cautious when using standard libraries and symbols, since they
 might not be available in all the development environments (especially, they might not be available
 in the final evaluation environment, preventing your code from compiling).
 This WARNING will be valid far ALL THE ASSIGNMENTs, but it will not be repeated in the following texts,
 so please remember these advices carefully!
 
***************/


// using silde of 06, we can use those codes to make this assignment.

glm::mat4 MakeViewProjectionLookInDirection(glm::vec3 Pos, float Yaw, float Pitch, float Roll, float FOVy, float Ar, float nearPlane, float farPlane) {
    // Create a View Projection Matrix with the following characteristics:
    // Projection:
    //    - Perspective with:
    //    - Fov-y defined in formal parameter >FOVy<
    //  - Aspect ratio defined in formal parameter >Ar<
    //  - Near Plane distance defined in formal parameter >nearPlane<
    //  - Far Plane distance defined in formal parameter >farPlane<
    // View:
    //    - Use the Look-In-Direction model with:
    //    - Camera Positon defined in formal parameter >Pos<
    //    - Looking direction defined in formal parameter >Yaw<
    //    - Looking elevation defined in formal parameter >Pitch<
    //    - Looking rool defined in formal parameter >Roll<
    //glm::mat4 M = glm::mat4(1.0f);
    glm::mat4 Port = glm::perspective(-FOVy, Ar, nearPlane, farPlane);
    glm::mat4 M =
        glm::rotate(glm::mat4(1.0), -Roll, glm::vec3(0, 0, 1)) *
        glm::rotate(glm::mat4(1.0), -Pitch, glm::vec3(1, 0, 0)) *
        glm::rotate(glm::mat4(1.0), -Yaw, glm::vec3(0, 1, 0)) *
        glm::translate(glm::mat4(1.0), -Pos);
    M = Port * M;
    return M;
}

glm::mat4 MakeViewProjectionLookAt(glm::vec3 Pos, glm::vec3 Target, glm::vec3 Up, float Roll, float FOVy, float Ar, float nearPlane, float farPlane) {
    // Create a View Projection Matrix with the following characteristics:
    // Projection:
    //    - Perspective with:
    //    - Fov-y defined in formal parameter >FOVy<
    //  - Aspect ratio defined in formal parameter >Ar<
    //  - Near Plane distance defined in formal parameter >nearPlane<
    //  - Far Plane distance defined in formal parameter >farPlane<
    // View:
    //    - Use the Look-At model with:
    //    - Camera Positon defined in formal parameter >Pos<
    //    - Camera Target defined in formal parameter >Target<
    //    - Up vector defined in formal parameter >Up<
    //    - Looking rool defined in formal parameter >Roll<
    glm::mat4 Port = glm::perspective(-FOVy, Ar, nearPlane, farPlane);
    glm::mat4 M = glm::rotate(glm::mat4(1.0), -Roll, glm::vec3(0, 0, 1)) *
    glm::lookAt(Pos, Target, Up);
    M = Port * M;
    return M;
}

glm::mat4 MakeWorld(glm::vec3 Pos, float Yaw, float Pitch, float Roll) {
    // Create a World Matrix with the following characteristics:
    //    - Object Positon defined in formal parameter >Pos<
    //    - Euler angle rotation yaw defined in formal parameter >Yaw<
    //    - Euler angle rotation pitch defined in formal parameter >Pitch<
    //    - Euler angle rotation roll defined in formal parameter >Roll<
    //  - Scaling constant and equal to 1 (and not passed to the procedure)
    
    glm::mat4 M =
        glm::translate(glm::mat4(1.0), Pos) *
        glm::rotate(glm::mat4(1.0), Yaw, glm::vec3(0, 1, 0)) *
        glm::rotate(glm::mat4(1.0), Pitch, glm::vec3(1, 0, 0)) *
        glm::rotate(glm::mat4(1.0), Roll, glm::vec3(0, 0, 1));

    return M;
}


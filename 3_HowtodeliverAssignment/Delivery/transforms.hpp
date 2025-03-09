

void SetTransform(A02 *A) {
/**************
 Using A->SetMatrix(i, M, focus) where:
 i -> index of the matrix to set (0 .. 14)
 M -> transform matrix to use
 focus -> true if you want to add the focus to that piece (only one please)
 
 solve the puzzle. Piece 0 is alread in place, and the correct transform to put in place piece 1, is
 the identity matrix. Find the correct transformations to place the other 13 pieces (i = 2..14).
 The suggestion is to work on the pieces one at a time, setting the focus to the one you are working on.
 The piece with the focus, will be displayed in red. Moreover, its origin, and the orientation and size
 of its axis will be shown. This is necessary because not all the pieces have their center in the origin,
 but remember that the transformations, assumes that the center is in the origin. Some pieces (especially
 the ones with a high index) will require a combination of transformations, since the transform is not
 centered with the origin or the main axes.
 
 WARNING!
 Since it is a C program, you can use for loops and functions if you think they can be helpful in your solution.
 However, please include all your code in this file, since it will be put in an automatic correction process
 for the final evaluation. Please also be cautious when using standard libraries and symbols, since they
 might not be available in all the development environments (especially, they might not be available
 in the final evaluation environment, preventing your code from compiling).
 This WARNING will be valid far ALL THE ASSIGNMENTs, but it will not be repeated in the following texts,
 so please remember these advices carefully!
 
***************/
	glm::mat4 M;

	// this is the only one in place, and that should not be modified
	M = glm::mat4(1.0f);
	A->SetMatrix(0,  M, false);

	// this is an example on how to highlight a piece and move it.
	// to put it in place, just replace the following line with an identity matrix,
	// that is: A->SetMatrix(0,  M, false);
	//M = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-0.3f, 0.0f, -1.6f)),
					//glm::radians(15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	M = glm::mat4(1.0f);			// to put this piece in places, uncomment this line and delete the two above
	A->SetMatrix(1,  M, false);

	// FROM THIS POINT ON, you have to find the correct transform to solve the puzzle.
    // notes: 2 is a metric
    M = glm::translate(glm::mat4(1.0f), glm::vec3(-6,0,-3)) * glm::scale(glm::mat4(1.0f), glm::vec3(3.0f));
	A->SetMatrix(2,  M, false);
	
    // rotate around the z-axis, down; translate along the z-axis, positive;
	M = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,1)) * glm::rotate(glm::mat4(1.0f), glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	A->SetMatrix(3,  M, false);
	
    // rotate around the z-axis 180 degreen, for change the piece direction.
	M = glm::translate(glm::mat4(1.0f), glm::vec3(2,0.1,1)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	A->SetMatrix(4,  M, false);
	
    // only need to translate
	M = glm::translate(glm::mat4(1.0f), glm::vec3(4,0,4));
	A->SetMatrix(5,  M, false);
    
	// i need to change the scale of the piece, it's not a normall size, so i tried so many times to match the other pieces, and the i change the translate to do some minor adjustments.
	M = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,-1.25)) * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f,1.0f,1.25f));
	A->SetMatrix(6,  M, false);
	
    
    /*
     Shear requires the inclusion of a special header, <glm/gtx/transform2.hpp>, and
     can be performed with the following procedures:
     – glm::mat4 Rx = glm::shearX3D(glm::mat4(1), hy, hz);
     – glm::mat4 Rx = glm::shearY3D(glm::mat4(1), hx, hz);
     – glm::mat4 Rx = glm::shearZ3D(glm::mat4(1), hx, hy);*/
    
    M = glm::translate(glm::mat4(1.0f), glm::vec3(2.5,0,-1)) * glm::shearX3D(glm::mat4(1), 0.0f, -0.8f) * glm::rotate(glm::mat4(1.0f), glm::radians(-39.0f), glm::vec3(0,1,0)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.28f,1.0f,0.8f));
	//M = glm::mat4(1.0f);
	A->SetMatrix(7,  M, false);
    
    M = glm::translate(glm::mat4(1.0f), glm::vec3(1.03,2,1.05)) * glm::scale(glm::mat4(1.0f), glm::vec3(2.23f,1.0f,1.39f)) * glm::shearX3D(glm::mat4(1), 0.0f, 1.3f) * glm::rotate(glm::mat4(1.0f), glm::radians(117.5f), glm::vec3(0,1,0));
	//M = glm::rotate(glm::mat4(1), glm::radians(50.0f), glm::vec3(0,1,0));
	A->SetMatrix(8,  M, false);
	
	M = glm::mat4(1.0f);
	A->SetMatrix(9,  M, false);
	
	M = glm::scale(glm::mat4(1), glm::vec3(2.0f,2.0f,2.0f))* glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(0,1,0)) * glm::translate(glm::mat4(1), glm::vec3(-1,1,1.5));
	A->SetMatrix(10, M, false);
	
    // i've found that we need to rotate first, after excuting this, we can directly change the x,y,z coordinates.
	M = glm::rotate(glm::mat4(1), glm::radians(135.0f), glm::vec3(0,1,0)) * glm::translate(glm::mat4(1.0f), glm::vec3(-2.708f,2.0f,-0.88f));
	A->SetMatrix(11, M, false);
	
    M = glm::scale(glm::mat4(1.0f), glm::vec3(1,1,0.5f)) * glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(0,1,0)) * glm::translate(glm::mat4(1), glm::vec3(1,2,0));
	A->SetMatrix(12, M, false);
	
    // rotate around an axis means rotating parallel to that axis with it as the central axis.
    M = glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(0,1,0)) * glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(1,0,0)) *  glm::rotate(glm::mat4(1), glm::radians(45.0f), glm::vec3(0,0,1)) * glm::rotate(glm::mat4(1), glm::radians(-135.0f), glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1), glm::vec3(-0.91f,-0.70f,0.92f));
    A->SetMatrix(13, M, false);
	
    //rotating clockwise is representated by a nagative number
	M = glm::rotate(glm::mat4(1.0f), glm::radians(150.0f), glm::vec3(0,1,0)) * glm::translate(glm::mat4(1), glm::vec3(2.235f,2,0.129));
	A->SetMatrix(14, M, false);
}

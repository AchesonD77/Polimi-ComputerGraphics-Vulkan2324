// This has been adapted from the Vulkan tutorial

// TO MOVE
#define JSON_DIAGNOSTICS 1
#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"


std::vector<SingleText> outText = {
	{1, {"Third Person", "Press SPACE to change view", "", ""}, 0, 0},
	{1, {"First Person", "Press SPACE to change view", "", ""}, 0, 0},
	{1, {"Saving Screenshots. Please wait.","","",""}, 0, 0},
	{2, {"Screenshot Saved!","Press ESC to quit","",""}, 0, 0}
};

// The uniform buffer object used in this example
struct UniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct GlobalUniformBufferObject {
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 eyePos;
	alignas(16) glm::vec4 eyeDir;
};



// The vertices data structures
// Example
struct Vertex {
	glm::vec3 pos;
	glm::vec2 UV;
	glm::vec3 norm;
};

#include "modules/Scene.hpp"

#include "WVP.hpp"


// MAIN ! 
class A04 : public BaseProject {
	protected:
	
	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSL;

	// Vertex formats
	VertexDescriptor VD;

	// Pipelines [Shader couples]
	Pipeline P;

	Scene SC;
	glm::vec3 **deltaP;
	float *deltaA;
	float *usePitch;

	TextMaker txt;
	
	// Other application parameters
	int currScene = 0;
	float Ar;
	glm::vec3 Pos;
	float Yaw;
	glm::vec3 InitialPos;
	
	std::vector<std::string> trkClEl = {"tb", "tdl", "tdr"};
	std::vector<std::string> trkOpEl = {"tbo", "tdlo", "tdro", "tswo"};
	std::vector<std::string> trkComEl = {"tfwl", "tfwr", "tmwl", "tmwr"};
	std::vector<std::string> trailerEl = {"tt", "tbwfl", "tbwfr", "tbwrl", "tbwrr"};
	std::vector<std::string> trkScn =  {"pln", "prm"};

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "A04 - World View Projection";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.85f, 1.0f, 1.0f};
		
		// Descriptor pool sizes
		uniformBlocksInPool = 19 * 2 + 2;
		texturesInPool = 19 + 1;
		setsInPool = 19 + 1;

		Ar = 4.0f / 3.0f;
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		std::cout << "Window resized to: " << w << " x " << h << "\n";
		Ar = (float)w / (float)h;
	}
	
	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]
		DSL.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
					{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
				});

		// Vertex descriptors
		VD.init(this, {
				  {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, UV),
				         sizeof(glm::vec2), UV},
				  {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, norm),
				         sizeof(glm::vec3), NORMAL}
				});

		// Pipelines [Shader couples]
		P.init(this, &VD, "shaders/PhongVert.spv", "shaders/PhongFrag.spv", {&DSL});
		P.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_NONE, false);

		// Models, textures and Descriptors (values assigned to the uniforms)
/*		std::vector<Vertex> vertices = {
					   {{-100.0,0.0f,-100.0}, {0.0f,0.0f}, {0.0f,1.0f,0.0f}},
					   {{-100.0,0.0f, 100.0}, {0.0f,1.0f}, {0.0f,1.0f,0.0f}},
					   {{ 100.0,0.0f,-100.0}, {1.0f,0.0f}, {0.0f,1.0f,0.0f}},
					   {{ 100.0,0.0f, 100.0}, {1.0f,1.0f}, {0.0f,1.0f,0.0f}}};
		M1.vertices = std::vector<unsigned char>(vertices.size()*sizeof(Vertex), 0);
		memcpy(&vertices[0], &M1.vertices[0], vertices.size()*sizeof(Vertex));
		M1.indices = {0, 1, 2,    1, 3, 2};
		M1.initMesh(this, &VD); */


		// Load Scene
		SC.init(this, &VD, DSL, P, "models/scene.json");
		
		// updates the text
		txt.init(this, &outText);

		// Init local variables
		Pos = glm::vec3(SC.I[SC.InstanceIds["tb"]].Wm[3]);
		InitialPos = Pos;
		Yaw = 0;
		
		deltaP = (glm::vec3 **)calloc(SC.InstanceCount, sizeof(glm::vec3 *));
		deltaA = (float *)calloc(SC.InstanceCount, sizeof(float));
		usePitch = (float *)calloc(SC.InstanceCount, sizeof(float));
		for(int i=0; i < SC.InstanceCount; i++) {
			deltaP[i] = new glm::vec3(SC.I[i].Wm[3]);
			deltaA[i] = 0.0f;
			usePitch[i] = 0.0f;
		}
		deltaA[SC.InstanceIds["tfwl"]] =
		deltaA[SC.InstanceIds["tmwl"]] =
		deltaA[SC.InstanceIds["tbwfl"]] =
		deltaA[SC.InstanceIds["tbwrl"]] =
						M_PI;
		usePitch[SC.InstanceIds["tfwl"]] =
		usePitch[SC.InstanceIds["tmwl"]] =
		usePitch[SC.InstanceIds["tbwfl"]] =
		usePitch[SC.InstanceIds["tbwrl"]] = -1.0f;
		usePitch[SC.InstanceIds["tfwr"]] =
		usePitch[SC.InstanceIds["tmwr"]] =
		usePitch[SC.InstanceIds["tbwfr"]] =
		usePitch[SC.InstanceIds["tbwrr"]] = 1.0f;
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		P.create();

		// Here you define the data set
		SC.pipelinesAndDescriptorSetsInit(DSL);
		txt.pipelinesAndDescriptorSetsInit();
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		P.cleanup();

		SC.pipelinesAndDescriptorSetsCleanup();
		txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {	
		for(int i=0; i < SC.InstanceCount; i++) {
			delete deltaP[i];
		}
		free(deltaP);
		free(deltaA);
		free(usePitch);

		// Cleanup descriptor set layouts
		DSL.cleanup();
		
		// Destroies the pipelines
		P.destroy();		

		SC.localCleanup();		
		txt.localCleanup();		
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		// binds the pipeline
		P.bind(commandBuffer);

/*		// binds the data set
		DS1.bind(commandBuffer, P, 0, currentImage);
					
		// binds the model
		M1.bind(commandBuffer);
		
		// record the drawing command in the command buffer
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);
std::cout << M1.indices.size();
*/
		SC.populateCommandBuffer(commandBuffer, currentImage, P);
		txt.populateCommandBuffer(commandBuffer, currentImage, currScene);
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		static bool debounce = false;
		static int curDebounce = 0;

		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);
		
		static float CamPitch = glm::radians(20.0f);
		static float CamYaw   = M_PI;
		static float CamDist  = 10.0f;
		static float CamRoll  = 0.0f;
		const glm::vec3 CamTargetDelta = glm::vec3(0,2,0);
		const glm::vec3 Cam1stPos = glm::vec3(0.49061f, 2.07f, 2.7445f);
		
		float TrailerAng = 0.0f;
		static float SteeringAng = 0.0f;
		static float wheelRoll = 0.0f;
		static float dampedVel = 0.0f;
		
// printVec3("m: ", m);
// printVec3("r: ", r);
		
		const float STEERING_SPEED = glm::radians(30.0f);
		const float ROT_SPEED = glm::radians(120.0f);
		const float MOVE_SPEED = 2.5f;

		SteeringAng += -m.x * STEERING_SPEED * deltaT;
		SteeringAng = (SteeringAng < glm::radians(-35.0f) ? glm::radians(-35.0f) :
					  (SteeringAng > glm::radians(35.0f)  ? glm::radians(35.0f)  : SteeringAng));
		
		
		const float trailerL = 4.5f;
		static float tx = Pos.x - trailerL * sin(Yaw + TrailerAng);
		static float tz = Pos.z - trailerL * cos(Yaw + TrailerAng);

		glm::mat4 M;
		glm::vec3 CamPos = Pos;
		static glm::vec3 dampedCamPos = CamPos;

		double lambdaVel = 8.0f;
		double dampedVelEpsilon = 0.001f;
		dampedVel =  MOVE_SPEED * deltaT * m.z * (1 - exp(-lambdaVel * deltaT)) +
					 dampedVel * exp(-lambdaVel * deltaT);
		dampedVel = ((fabs(dampedVel) < dampedVelEpsilon) ? 0.0f : dampedVel);
		wheelRoll -= dampedVel / 0.4;
		wheelRoll = (wheelRoll < 0.0 ? wheelRoll + 2*M_PI : (wheelRoll > 2*M_PI ? wheelRoll - 2*M_PI : wheelRoll));

		if(dampedVel != 0.0f) {
			glm::vec3 trailerPos = glm::vec3(tx, 0.0f, tz);
			glm::vec3 oldPos = Pos;
			
			if(SteeringAng != 0.0f) {
				const float l = 2.78f;
				float r = l / tan(SteeringAng);
				float cx = Pos.x + r * cos(Yaw);
				float cz = Pos.z - r * sin(Yaw);
				float Dbeta = dampedVel / r;
				Yaw = Yaw - Dbeta;
				Pos.x = cx - r * cos(Yaw);
				Pos.z = cz + r * sin(Yaw);
			} else {
				Pos.x = Pos.x - sin(Yaw) * dampedVel;
				Pos.z = Pos.z - cos(Yaw) * dampedVel;
			}
			if(m.x == 0) {
				if(SteeringAng > STEERING_SPEED * deltaT) {
					SteeringAng -= STEERING_SPEED * deltaT;
				} else if(SteeringAng < -STEERING_SPEED * deltaT) {
					SteeringAng += STEERING_SPEED * deltaT;
				} else {
					SteeringAng = 0.0f;
				}					
			}

			glm::vec3 deltaPos = Pos - oldPos;
			glm::vec3 trailerDir = glm::normalize(Pos - trailerPos);
			glm::vec3 trailerMove = glm::dot(deltaPos, trailerDir) * trailerDir;
//printVec3("v * DeltaT",deltaPos);
//printVec3("trailerDir",trailerDir);
//printVec3("trailerMov",trailerMove);
			glm::vec3 preTrailerPos = trailerPos + trailerMove;
			glm::vec3 newTrailerDir = glm::normalize(preTrailerPos - Pos);
			trailerPos = Pos + trailerL * newTrailerDir;
			tx = trailerPos.x;
			tz = trailerPos.z;
//std::cout << Yaw << " " << TrailerAng << "\n";
			
/*			glm::vec3 trailerDir = glm::normalize(Pos - trailerPos);
printVec3("TD",trailerDir);
			glm::vec3 newTrailerPos = trailerPos + trailerDir * dot(trailerDir, deltaPos);
			float newTrailerDir = acos(glm::dot(glm::normalize(newTrailerPos - Pos), glm::vec3(sin(Yaw),0,cos(Yaw))));
std::cout << newTrailerDir << "\n";
			TrailerAng = newTrailerDir;
*/			
//			SC.I[SC.InstanceIds["ax"]].Wm = glm::translate(glm::mat4(1.0f), trailerPos);
		}
		TrailerAng = atan2(Pos.x-tx, Pos.z-tz);
		

		static int subpass = 0;
		static float subpassTimer = 0.0;


		if(glfwGetKey(window, GLFW_KEY_SPACE)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_SPACE;
				if(currScene != 2) {
					currScene = (currScene+1) % outText.size();
					if(currScene == 0) {
						CamPitch = glm::radians(20.0f);
						CamYaw   = M_PI;
						CamRoll  = 0.0f;
						dampedCamPos = Pos;
					} else if(currScene == 1) {
						CamPitch = glm::radians(0.0f);
						CamYaw   = M_PI;
						CamRoll  = 0.0f;
					}

				}
				if(currScene == 2) {
					if(subpass >= 8) {
						currScene = 0;
					}
				}
				std::cout << "Scene : " << currScene << "\n";

				RebuildPipeline();
			}
		} else {
			if((curDebounce == GLFW_KEY_SPACE) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		// Standard procedure to quit when the ESC key is pressed
		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}


		if(glfwGetKey(window, GLFW_KEY_V)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_V;

				printVec3("Pos = ", Pos);				
				std::cout << "Yaw         = " << Yaw         << ";\n";
				std::cout << "CamPitch    = " << CamPitch    << ";\n";
				std::cout << "CamYaw      = " << CamYaw      << ";\n";
				std::cout << "CamRoll     = " << CamRoll     << ";\n";
				std::cout << "CamDist     = " << CamDist     << ";\n";
				std::cout << "SteeringAng = " << SteeringAng << ";\n";
				std::cout << "wheelRoll   = " << wheelRoll   << ";\n";
				std::cout << "tx   = " << tx   << ";\n";
				std::cout << "tz   = " << tz   << ";\n\n";
			}
		} else {
			if((curDebounce == GLFW_KEY_V) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		if(currScene == 2) {
			switch(subpass) {
			  case 0:
Pos  = glm::vec3(0, 0, 0);
Yaw         = 0;
CamPitch    = 0.349066;
CamYaw      = 3.14159;
CamRoll     = 0;
CamDist     = 10;
SteeringAng = 0;
wheelRoll   = 0;
tx   = 0;
tz   = -4.5;

				break;
			  case 1:
Pos   = glm::vec3(0.650061, 0, 4.53863);
Yaw         = 0.519612;
CamPitch    = 0;
CamYaw      = 1.94458;
CamRoll     = 0.00288426;
CamDist     = 7;
SteeringAng = 0.443557;
wheelRoll   = 5.31861;
tx   = 0.115473;
tz   = 0.0705009;
				break;
			  case 2:
Pos   = glm::vec3(0.650061, 0, 4.53863);
Yaw         = 0.519612;
CamPitch    = 0.732941;
CamYaw      = 1.38603;
CamRoll     = 0.00288426;
CamDist     = 8.28082;
SteeringAng = 0.443557;
wheelRoll   = 5.31861;
tx   = 0.115473;
tz   = 0.0705009;
				break;
			  case 3:
Pos   = glm::vec3(0.650061, 0, 4.53863);
Yaw         = 0.519612;
CamPitch    = 0.732941;
CamYaw      = 1.38603;
CamRoll     = -0.89825;
CamDist     = 8.28082;
SteeringAng = 0.443557;
wheelRoll   = 5.31861;
tx   = 0.115473;
tz   = 0.0705009;

				break;
			  case 4:
Pos   = glm::vec3(14.2882, 0, 19.2075);
Yaw         = -0.0802393;
CamPitch    = 0;
CamYaw      = 3.14159;
CamRoll     = 0;
CamDist     = 7;
SteeringAng = -0.610865;
wheelRoll   = 2.31093;
tx   = 11.1696;
tz   = 15.9633;
				break;
			  case 5:
Pos   = glm::vec3(9.47949, 0, 26.1118);
Yaw         = -1.12858;
CamPitch    = -0.399017;
CamYaw      = 3.14159;
CamRoll     = 0;
CamDist     = 7;
SteeringAng = 0;
wheelRoll   = 5.58524;
tx   = 11.7015;
tz   = 22.1987;
				break;
			  case 6:
Pos   = glm::vec3(9.47949, 0, 26.1118);
Yaw         = -1.12858;
CamPitch    = -0.399017;
CamYaw      = 3.8619;
CamRoll     = 0;
CamDist     = 7;
SteeringAng = 0;
wheelRoll   = 5.58524;
tx   = 11.7015;
tz   = 22.1987;
				break;
			  case 7:
Pos   = glm::vec3(9.47949, 0, 26.1118);
Yaw         = -1.12858;
CamPitch    = -0.303572;
CamYaw      = 2.68474;
CamRoll     = -1.45504;
CamDist     = 7;
SteeringAng = -0.109526;
wheelRoll   = 5.58524;
tx   = 11.7015;
tz   = 22.1987;
				break;
			}
		}
		
		if((currScene == 0) || ((currScene == 2) && (subpass < 4))) {
			CamYaw += ROT_SPEED * deltaT * r.y;
			CamPitch -= ROT_SPEED * deltaT * r.x;
			CamRoll -= ROT_SPEED * deltaT * r.z;
			CamDist -= MOVE_SPEED * deltaT * m.y;
		
			CamYaw = (CamYaw < 0.0f ? 0.0f : (CamYaw > 2*M_PI ? 2*M_PI : CamYaw));
			CamPitch = (CamPitch < 0.0f ? 0.0f : (CamPitch > M_PI_2-0.01f ? M_PI_2-0.01f : CamPitch));
			CamRoll = (CamRoll < -M_PI ? -M_PI : (CamRoll > M_PI ? M_PI : CamRoll));
			CamDist = (CamDist < 7.0f ? 7.0f : (CamDist > 15.0f ? 15.0f : CamDist));
				
			glm::vec3 CamTarget = Pos + glm::vec3(glm::rotate(glm::mat4(1), Yaw, glm::vec3(0,1,0)) *
							 glm::vec4(CamTargetDelta,1));
			CamPos = CamTarget + glm::vec3(glm::rotate(glm::mat4(1), Yaw + CamYaw, glm::vec3(0,1,0)) * glm::rotate(glm::mat4(1), -CamPitch, glm::vec3(1,0,0)) * 
							 glm::vec4(0,0,CamDist,1));

			const float lambdaCam = 10.0f;
			dampedCamPos = CamPos * (1 - exp(-lambdaCam * deltaT)) +
						 dampedCamPos * exp(-lambdaCam * deltaT); 
			M = MakeViewProjectionLookAt(dampedCamPos, CamTarget, glm::vec3(0,1,0), CamRoll, glm::radians(90.0f), Ar, 0.1f, 500.0f);
		} else if((currScene == 1) || ((currScene == 2) && (subpass >= 4))) {
			CamYaw -= ROT_SPEED * deltaT * r.y;
			CamPitch -= ROT_SPEED * deltaT * r.x;
			CamRoll -= ROT_SPEED * deltaT * r.z;
		
			CamYaw = (CamYaw < M_PI_2 ? M_PI_2 : (CamYaw > 1.5*M_PI ? 1.5*M_PI : CamYaw));
			CamPitch = (CamPitch < -0.25*M_PI ? -0.25*M_PI : (CamPitch > 0.25*M_PI ? 0.25*M_PI : CamPitch));
			CamRoll = (CamRoll < -M_PI ? -M_PI : (CamRoll > M_PI ? M_PI : CamRoll));
				
			glm::vec3 Cam1Pos = Pos + glm::vec3(glm::rotate(glm::mat4(1), Yaw, glm::vec3(0,1,0)) *
							 glm::vec4(Cam1stPos,1));
			M = MakeViewProjectionLookInDirection(Cam1Pos, Yaw + CamYaw, CamPitch, CamRoll, glm::radians(90.0f), Ar, 0.1f, 500.0f);
		} else {
			M = glm::perspective(glm::radians(45.0f), Ar, 0.1f, 500.0f);
			M[1][1] *= -1;			
		}
		if(currScene == 2) {
			subpassTimer += deltaT;
			if(subpassTimer > 1.0f) {
				subpassTimer = 0.0f;
				subpass++;
				std::cout << "Scene : " << currScene << " subpass: " << subpass << "\n";
				char buf[20];
				sprintf(buf, "A04_%d.png", subpass);
				saveScreenshot(buf, currentImage);
				if(subpass == 8) {
					currScene++;
					std::cout << "Scene : " << currScene << "\n";
					RebuildPipeline();
				}
			}
		}

		glm::mat4 ViewPrj =  M;
		UniformBufferObject ubo{};
		glm::mat4 baseTr = glm::mat4(1.0f);								
		// Here is where you actually update your uniforms

		// updates global uniforms
		GlobalUniformBufferObject gubo{};
		gubo.lightDir = glm::vec3(cos(glm::radians(135.0f)), sin(glm::radians(135.0f)), 0.0f);
		gubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.eyePos = dampedCamPos;
		gubo.eyeDir = glm::vec4(0);
		gubo.eyeDir.w = 1.0;

//		for(int i = 0; i < SC.InstanceCount; i++) {
		// Draw the truck
		if(currScene == 0) {
			for (std::vector<std::string>::iterator it = trkClEl.begin(); it != trkClEl.end(); it++) {
				int i = SC.InstanceIds[it->c_str()];
				glm::vec3 dP = glm::vec3(glm::rotate(glm::mat4(1), Yaw, glm::vec3(0,1,0)) *
										 glm::vec4(*deltaP[i],1));
				ubo.mMat = MakeWorld(Pos + dP, Yaw + deltaA[i], usePitch[i] * wheelRoll, 0) * baseTr;
				ubo.mvpMat = ViewPrj * ubo.mMat;
				ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));

				SC.DS[i]->map(currentImage, &ubo, sizeof(ubo), 0);
				SC.DS[i]->map(currentImage, &gubo, sizeof(gubo), 2);
			}
			for (std::vector<std::string>::iterator it = trkOpEl.begin(); it != trkOpEl.end(); it++) {
				int i = SC.InstanceIds[it->c_str()];
				ubo.mMat = glm::mat4(0);
				ubo.mvpMat = glm::mat4(0);
				ubo.nMat = glm::mat4(0);

				SC.DS[i]->map(currentImage, &ubo, sizeof(ubo), 0);
				SC.DS[i]->map(currentImage, &gubo, sizeof(gubo), 2);
			}
		} else {
			for (std::vector<std::string>::iterator it = trkOpEl.begin(); it != trkOpEl.end(); it++) {
				int i = SC.InstanceIds[it->c_str()];
				float extraYaw = ((i == SC.InstanceIds["tswo"]) ? -SteeringAng * 15.0f: 0);
				float extraPitch = ((i == SC.InstanceIds["tswo"]) ? glm::radians(20.0f) : 0);
				glm::vec3 dP = glm::vec3(glm::rotate(glm::mat4(1), Yaw, glm::vec3(0,1,0)) *
										 glm::vec4(*deltaP[i],1));
				ubo.mMat = MakeWorld(Pos + dP, Yaw + deltaA[i], usePitch[i] * wheelRoll + extraPitch, extraYaw) * baseTr;
				ubo.mvpMat = ViewPrj * ubo.mMat;
				ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));

				SC.DS[i]->map(currentImage, &ubo, sizeof(ubo), 0);
				SC.DS[i]->map(currentImage, &gubo, sizeof(gubo), 2);
			}
			for (std::vector<std::string>::iterator it = trkClEl.begin(); it != trkClEl.end(); it++) {
				int i = SC.InstanceIds[it->c_str()];
				ubo.mMat = glm::mat4(0);
				ubo.mvpMat = glm::mat4(0);
				ubo.nMat = glm::mat4(0);

				SC.DS[i]->map(currentImage, &ubo, sizeof(ubo), 0);
				SC.DS[i]->map(currentImage, &gubo, sizeof(gubo), 2);
			}
		}
		for (std::vector<std::string>::iterator it = trkComEl.begin(); it != trkComEl.end(); it++) {
			int i = SC.InstanceIds[it->c_str()];
			float extraYaw = (((i == SC.InstanceIds["tfwl"]) || (i == SC.InstanceIds["tfwr"])) ? SteeringAng : 0);
			glm::vec3 dP = glm::vec3(glm::rotate(glm::mat4(1), Yaw, glm::vec3(0,1,0)) *
						             glm::vec4(*deltaP[i],1));
			ubo.mMat = MakeWorld(Pos + dP, Yaw + deltaA[i] + extraYaw, usePitch[i] * wheelRoll, 0) * baseTr;
			ubo.mvpMat = ViewPrj * ubo.mMat;
			ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));

			SC.DS[i]->map(currentImage, &ubo, sizeof(ubo), 0);
			SC.DS[i]->map(currentImage, &gubo, sizeof(gubo), 2);
		}
		// Draw the trailer
		for (std::vector<std::string>::iterator it = trailerEl.begin(); it != trailerEl.end(); it++) {
			int i = SC.InstanceIds[it->c_str()];
			glm::vec3 dP = glm::vec3(glm::rotate(glm::mat4(1), TrailerAng, glm::vec3(0,1,0)) *
						             glm::vec4(*deltaP[i],1));
			ubo.mMat = MakeWorld(Pos + dP, TrailerAng + deltaA[i], usePitch[i] * wheelRoll, 0) * baseTr;
			ubo.mvpMat = ViewPrj * ubo.mMat;
			ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));

			SC.DS[i]->map(currentImage, &ubo, sizeof(ubo), 0);
			SC.DS[i]->map(currentImage, &gubo, sizeof(gubo), 2);
		}
		// Draw the landscape
		for (std::vector<std::string>::iterator it = trkScn.begin(); it != trkScn.end(); it++) {
			int i = SC.InstanceIds[it->c_str()];
//std::cout << *it << " " << i << "\n";
			ubo.mMat = SC.I[i].Wm * baseTr;
			ubo.mvpMat = ViewPrj * ubo.mMat;
			ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));

			SC.DS[i]->map(currentImage, &ubo, sizeof(ubo), 0);
			SC.DS[i]->map(currentImage, &gubo, sizeof(gubo), 2);
		}
	}
};

// This is the main: probably you do not need to touch this!
int main() {
    A04 app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
// This has been adapted from the Vulkan tutorial

#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"

std::vector<SingleText> outText = {
	{4, {"Press SPACE to save the screenshot", "01234567890123456789", "01234567890123456789", "01234567890123456789"}, 0, 0},
	{2, {"Screenshot Saved!","Press ESC to quit","",""}, 0, 0}
};



// The vertices data structures
// Example
struct Vertex {
	glm::vec3 pos;
	glm::vec2 UV;
	glm::vec4 color;
};



struct Prim {
	int type;		// 1 - point, 2 - line, 3 - triangle
	float x1, y1;
	float x2, y2;
	float x3, y3;
	float r, g, b;
	
	void print() {
		std::cout << ((type == 1) ? "Point\t" : ((type == 2) ? "Line\t" : "Triangle\t"));
		std::cout << x1 << " " << y1 << "\t";
		if(type > 1) {std::cout << x2 << " " << y2 << "\t";}
		if(type > 2) {std::cout << x3 << " " << y3 << "\t";}
		std::cout << "[" << r << "," << g << "," << b << "]\n";
	};
};

class A01;
void DrawFigure(A01 *A);


// MAIN ! 
class A01 : public BaseProject {
	std::vector<Prim> primitives;
	public:
	
	void Plot(float x1, float y1, float r, float g, float b) {
		Prim t;
		t.type = 1;
		t.x1 = x1;
		t.y1 = y1;
		t.r = r;
		t.g = g;
		t.b = b;
		
		primitives.push_back(t);
	}

	void Line(float x1, float y1, float x2, float y2, float r, float g, float b) {
		Prim t;
		t.type = 2;
		t.x1 = x1;
		t.y1 = y1;
		t.x2 = x2;
		t.y2 = y2;
		t.r = r;
		t.g = g;
		t.b = b;
		
		primitives.push_back(t);
	}

	void Triangle(float x1, float y1, float x2, float y2, float x3, float y3, float r, float g, float b) {
		Prim t;
		t.type = 3;
		t.x1 = x1;
		t.y1 = y1;
		t.x2 = x2;
		t.y2 = y2;
		t.x3 = x3;
		t.y3 = y3;
		t.r = r;
		t.g = g;
		t.b = b;
		
		primitives.push_back(t);
	}
	protected:

	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSL;

	// Vertex formats
	VertexDescriptor VD;

	// Pipelines [Shader couples]
	Pipeline P;

	// Models, textures and Descriptors (values assigned to the uniforms)
	// Please note that Model objects depends on the corresponding vertex structure
	// Models
	Model<Vertex> M1, M2;
	// Descriptor sets
	DescriptorSet DS1;
	// Textures
	Texture T1;

	TextMaker txt;
	
	// Other application parameters
	int currScene = 0;

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "A01 - 2D coordinates";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.005f, 0.01f, 1.0f};
		
		// Descriptor pool sizes
		uniformBlocksInPool = 4;
		texturesInPool = 4;
		setsInPool = 4;
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) {
	}
	
	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]
		DSL.init(this, {
					{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
				});

		// Vertex descriptors
		VD.init(this, {
				  {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, UV),
				         sizeof(glm::vec2), UV},
				  {0, 2, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color),
				         sizeof(glm::vec4), COLOR}
				});

		// Pipelines [Shader couples]
		P.init(this, &VD, "shaders/ShaderVert.spv", "shaders/ShaderFrag.spv", {&DSL});
		P.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_NONE, false);
		// Models, textures and Descriptors (values assigned to the uniforms)
		M1.vertices = {{{-1.0,-1.0,0.9f}, {0.0f,0.0f}, {1.0f,1.0f,1.0f,0.83f}},
					   {{-1.0, 1.0,0.9f}, {0.0f,1.0f}, {1.0f,1.0f,1.0f,0.83f}},
					   {{ 1.0,-1.0,0.9f}, {1.0f,0.0f}, {1.0f,1.0f,1.0f,0.83f}},
					   {{ 1.0, 1.0,0.9f}, {1.0f,1.0f}, {1.0f,1.0f,1.0f,0.83f}}};
		M1.indices = {0, 1, 2,    1, 3, 2};
		M1.initMesh(this, &VD);

		DrawFigure(this);
		float z = 0.89999f;
		int j = 0;
		const float psize = 1.0f/150.0f;
		int Tcnt = 0, Pcnt = 0, Lcnt = 0;
		for(auto& t : primitives) {
			t.print();

			switch(t.type) {
			  case 1:
			  {
				Vertex v1a{{t.x1-psize, t.y1, z}, {0.0f,0.0f}, {t.r, t.g, t.b, 1.0f}};
				M2.vertices.push_back(v1a);
				Vertex v1b{{t.x1, t.y1-psize, z}, {0.0f,0.0f}, {t.r, t.g, t.b, 1.0f}};
				M2.vertices.push_back(v1b);
				Vertex v1c{{t.x1+psize, t.y1, z}, {0.0f,0.0f}, {t.r, t.g, t.b, 1.0f}};
				M2.vertices.push_back(v1c);
				Vertex v1d{{t.x1, t.y1+psize, z}, {0.0f,0.0f}, {t.r, t.g, t.b, 1.0f}};
				M2.vertices.push_back(v1d);
				
				M2.indices.push_back(j);
				M2.indices.push_back(j+1);
				M2.indices.push_back(j+2);
				M2.indices.push_back(j+0);
				M2.indices.push_back(j+2);
				M2.indices.push_back(j+3);
				j+=4;

				Pcnt++;
			  }
			    break;
			  case 2:
			  {
				float a = atan2(t.y2-t.y1, t.x2-t.x1) + 3.1416/2.0f;
				float dx = psize * cos(a);
				float dy = psize * sin(a);
				
				Vertex v1a{{t.x1-dx, t.y1-dy, z}, {0.0f,0.0f}, {t.r, t.g, t.b, 1.0f}};
				M2.vertices.push_back(v1a);
				Vertex v1b{{t.x1+dx, t.y1+dy, z}, {0.0f,0.0f}, {t.r, t.g, t.b, 1.0f}};
				M2.vertices.push_back(v1b);
				Vertex v2a{{t.x2-dx, t.y2-dy, z}, {0.0f,0.0f}, {t.r, t.g, t.b, 1.0f}};
				M2.vertices.push_back(v2a);
				Vertex v2b{{t.x2+dx, t.y2+dy, z}, {0.0f,0.0f}, {t.r, t.g, t.b, 1.0f}};
				M2.vertices.push_back(v2b);
				
				M2.indices.push_back(j);
				M2.indices.push_back(j+1);
				M2.indices.push_back(j+2);
				M2.indices.push_back(j+1);
				M2.indices.push_back(j+2);
				M2.indices.push_back(j+3);
				j+=4;

				Lcnt++;
			  }
				break;
			  case 3:
			  {
				Vertex v1{{t.x1, t.y1, z}, {0.0f,0.0f}, {t.r, t.g, t.b, 1.0f}};
				M2.vertices.push_back(v1);
				M2.indices.push_back(j++);
				
				Vertex v2{{t.x2, t.y2, z}, {0.0f,0.0f}, {t.r, t.g, t.b, 1.0f}};
				M2.vertices.push_back(v2);
				M2.indices.push_back(j++);
				
				Vertex v3{{t.x3, t.y3, z}, {0.0f,0.0f}, {t.r, t.g, t.b, 1.0f}};
				M2.vertices.push_back(v3);
				M2.indices.push_back(j++);
				
				Tcnt++;
			  }
				break;
			}
			z -= 0.00001;
		}
		M2.initMesh(this, &VD);
		
		// Create the textures
		T1.init(this,   "textures/Checker.png");

		// updates the text
		outText[0].l[1] = (char *)malloc(200);
		sprintf((char *)outText[0].l[1], "Pnt: %d",Pcnt);
		outText[0].l[2] = (char *)malloc(200);
		sprintf((char *)outText[0].l[2], "Lin: %d",Lcnt);
		outText[0].l[3] = (char *)malloc(200);
		sprintf((char *)outText[0].l[3], "Tri: %d",Tcnt);
		txt.init(this, &outText);

		// Init local variables
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		P.create();

		// Here you define the data set
		DS1.init(this, &DSL, {
					{0, TEXTURE, 0, &T1}
				});
		txt.pipelinesAndDescriptorSetsInit();
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		P.cleanup();

		// Cleanup datasets
		DS1.cleanup();
		txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {
		// Cleanup textures
		T1.cleanup();
		
		// Cleanup models
		M1.cleanup();
		M2.cleanup();
		
		// Cleanup descriptor set layouts
		DSL.cleanup();
		
		// Destroies the pipelines
		P.destroy();		
		txt.localCleanup();
		
		free((char *)outText[0].l[1]);
		free((char *)outText[0].l[2]);
		free((char *)outText[0].l[3]);
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		// binds the pipeline
		P.bind(commandBuffer);

		// binds the data set
		DS1.bind(commandBuffer, P, 0, currentImage);
					
		// binds the model
		M1.bind(commandBuffer);
		
		// record the drawing command in the command buffer
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		// binds the model
		M2.bind(commandBuffer);
		
		// record the drawing command in the command buffer
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M2.indices.size()), 1, 0, 0, 0);

		txt.populateCommandBuffer(commandBuffer, currentImage, currScene);
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		static bool debounce = false;
		static int curDebounce = 0;

		if(glfwGetKey(window, GLFW_KEY_SPACE)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_SPACE;
				currScene = (currScene+1) % 2;
				std::cout << "Scene : " << currScene << "\n";

				if(!screenshotSaved) {
					saveScreenshot("A01.png", currentImage);
				}

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
	}
};

#include "figure.hpp"

// This is the main: probably you do not need to touch this!
int main() {
    A01 app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
//Internal
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>

#include <algorithm>
#include <stdbool.h>
#include <map>
#include <chrono>

//External
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

//Specific
#include <wndwfunctions.h>


int main() {

	//Initialization
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(1300, 1000, "Nginx Analyzer", nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsLight();

	std::ifstream fsLogFile("access.log");
	std::string sBuffer0;

	std::vector<std::vector<std::string>> vLogData;


	int time_counter = 0;


	if (fsLogFile.is_open()) {

		while (std::getline(fsLogFile, sBuffer0)) {

			std::vector<std::string> vRow;
			std::stringstream sStream0(sBuffer0);
			std::string sCell;


			while (std::getline(sStream0, sCell, '|')) {

				vRow.push_back(sCell);

			}
			vLogData.push_back(vRow);
		}

		fsLogFile.close();
	}
	else
	{
		std::cerr << "Could not open log file" << std::endl;
	}


	std::vector<int> FilterIndexes;
	int IP_Amount = 0;
	std::string IP_Selected;
	std::unordered_set<std::string> Requests;

while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

			ImGui::Begin("Addresses"); 


			ImGui::Text("Need to have:\n Unique Addresses,\n Total connections sorted high to low\n\n");

			std::unordered_set<std::string> ussUniqueSourceAddresses = ussSourceAddressesUnique(&vLogData);
			IP_Amount = ussUniqueSourceAddresses.size();
			static int iSelectedRow = -1; 




			for (std::string Address : ussUniqueSourceAddresses) {


				if (ImGui::Selectable(Address.c_str())) {

					Requests.clear();
					FilterIndexes.clear();

					for (int i = 0; i < vLogData.size(); i++) {
						if (Address == vLogData[i][9]) { 
									

							FilterIndexes.push_back(i);

						}
					}
				}
			}
				


			ImGui::End();

			ImGui::Begin("Hello, world!");

			ImGui::Text("This is Main window where all changes happen");


			std::vector vHeaders = vLogData[0];
			
			ImGui::BeginTable("Network Logs", vHeaders.size());

			for (int i = 0; i < vHeaders.size(); i++) { 
				ImGui::TableSetupColumn(vHeaders[i].c_str());
			}
			ImGui::TableHeadersRow();

			int IP_Connections = 0;

			for (int Filter : FilterIndexes) {
				
				std::vector <std::string> vsRowData = vLogData[Filter];
				IP_Connections++;
				ImGui::TableNextRow();

				int iColumn = 0;
				for (std::string sData : vsRowData) {

					ImGui::TableSetColumnIndex(iColumn);

					ImGui::Text(sData.c_str());

					if (iColumn == 4) {

						Requests.insert(sData.c_str());
					}

					if (iColumn == 9) {
						IP_Selected = sData;
					}

					iColumn++;
					
				}
			}

			ImGui::EndTable();

			ImGui::End();


			ImGui::Begin("Statistics");


			ImGui::BeginTabBar("IP Stats");
			if (ImGui::BeginTabItem("Overall Stats")) {

				ImGui::Text("Unique IP Addresses: %d \n", IP_Amount);
				ImGui::Text("Most connected IP: ");

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Selected IP")) {

				ImGui::Text("Selected IP Address : %s", IP_Selected.c_str());

				ImGui::Text("%s has connected %d time(s) this period \n", IP_Selected.c_str(), IP_Connections);


				ImGui::BeginTabBar("IP Stats");
				if (ImGui::BeginTabItem("Actions")) {

					for (std::string Actions : Requests) {

						ImGui::Text("%s \n", Actions.c_str());

					}

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();

				ImGui::EndTabItem();
			}
			
			
			ImGui::EndTabBar();

			ImGui::End();

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);
			glfwPollEvents();

		}
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		ImPlot::DestroyContext();
		glfwDestroyWindow(window);
		glfwTerminate();
		
	
	return 0;
}
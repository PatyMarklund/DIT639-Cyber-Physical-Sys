/*
 * Original file by Christian Berger "christian.berger@gu.se", modified by Group 14
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Include the single-file, header-only middleware libcluon to create high-performance microservices
#include "cluon-complete.hpp"
// Include the OpenDLV Standard Message Set that contains messages that are usually exchanged for automotive or robotic applications 
#include "opendlv-standard-message-set.hpp"

// Include the GUI and image processing header files from OpenCV
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core.hpp>

//Include Eigen, vector and others for algoritm
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <algorithm>
#include "eigen-3.4.0/Eigen/Dense"

//Declare variables used for testing purposes
float totalCorrectFrames = 0;
float totalFrames = 0;
float correctSteering = 0;
float differenceSteering = 0;

//Declare variables needed
float estimatedSteering = 0;
float angX = 0;
float angZ = 0;
float accX = 0;
float accZ = 0;

//Declare functions
bool load_input_data(const std::string& file_name, Eigen::MatrixXd& input_data);
bool load_data(const std::string& file_name, Eigen::MatrixXd& X, Eigen::VectorXd& y);
Eigen::VectorXd linear_regression(const Eigen::MatrixXd& X, const Eigen::VectorXd& y);
void save_results(const std::string& file_name, const Eigen::VectorXd& predictions);
void combine_files(const std::string& file_1, const std::string& file_2, const std::string& combined);
void apply_min_max(float& estimation);
void apply_doubling_estimation(float& estimation);

int32_t main(int32_t argc, char **argv) {
    int32_t retCode{1};
    // Parse the command line parameters as we require the user to specify some mandatory information on startup.
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if ( (0 == commandlineArguments.count("cid")) ||
         (0 == commandlineArguments.count("name")) ||
         (0 == commandlineArguments.count("width")) ||
         (0 == commandlineArguments.count("height")) ) {
        std::cerr << argv[0] << " attaches to a shared memory area containing an ARGB image." << std::endl;
        std::cerr << "Usage:   " << argv[0] << " --cid=<OD4 session> --name=<name of shared memory area> [--verbose]" << std::endl;
        std::cerr << "         --cid:    CID of the OD4Session to send and receive messages" << std::endl;
        std::cerr << "         --name:   name of the shared memory area to attach" << std::endl;
        std::cerr << "         --width:  width of the frame" << std::endl;
        std::cerr << "         --height: height of the frame" << std::endl;
        std::cerr << "Example: " << argv[0] << " --cid=253 --name=img --width=640 --height=480 --verbose" << std::endl;
    }
    else {
        // Extract the values from the command line parameters
        const std::string NAME{commandlineArguments["name"]};
        const uint32_t WIDTH{static_cast<uint32_t>(std::stoi(commandlineArguments["width"]))};
        const uint32_t HEIGHT{static_cast<uint32_t>(std::stoi(commandlineArguments["height"]))};
        const bool VERBOSE{commandlineArguments.count("verbose") != 0};
        const bool PIPELINE{commandlineArguments.count("pipeline") != 0};


        //Declare matrixes
        Eigen::MatrixXd X, input_data;
        Eigen::VectorXd y;

        // Load data for multiple linear regression
        if (!load_data("data-regression.csv", X, y)) {
            return 1;
        }

        //Calculate coefficients with linear regression
        Eigen::VectorXd coefficients = linear_regression(X, y);

        //DO CODE HERE IF --PIPELINE
        if (PIPELINE) {
            std::cout << "pipeline running" << std::endl;

            if (!load_input_data("input-data.csv", input_data)) {
                return 1;
            }
            
            // Compute predictions for the input data
            Eigen::VectorXd predictions = input_data * coefficients;
            save_results("/output/output_prediction.csv", predictions);

            combine_files("true_values_33.csv", "/output/output_prediction.csv", "/output/result_predictions_one.csv");
            combine_files("/output/result_predictions_one.csv", "/output/output_prediction_old.csv", "/output/result_predictions.csv");

            return 0;
        }

        //TO-DO
        //IN CODE FILE
        //LOAD FILE                 DONE
        //MAKE ESTIMATIONS          DONE
        //Check if multiplication   50% 
            //Update normal code to use new function
            //Update pipeline code to run function for each row        
        //Check max min             50%
            //Update normal code to use new function
            //Update pipeline code to run function for each row   
        //SAVE ESTIMATIONS          DONE
        //COMBINE WITH OLD AND CORR 0%
        //QUIT PROGRAM              DONE

        //IN PIPELINE
        //PLOT estimation file      DONE
        //PLOT estimation + correct DONE
        //plot est + corr + last    50%
        //REPEAT FOR ALL RECs       0%
        //EXPORT DIAGRAM            DONE

        // Attach to the shared memory.
        std::unique_ptr<cluon::SharedMemory> sharedMemory{new cluon::SharedMemory{NAME}};
        if (sharedMemory && sharedMemory->valid()) {
            std::clog << argv[0] << ": Attached to shared memory '" << sharedMemory->name() << " (" << sharedMemory->size() << " bytes)." << std::endl;

            // Interface to a running OpenDaVINCI session where network messages are exchanged.
            // The instance od4 allows you to send and receive messages.
            cluon::OD4Session od4{static_cast<uint16_t>(std::stoi(commandlineArguments["cid"]))};

            //Get GroundSteeringRequest from envelope - Used for testing purposes
            //Using https://github.com/chalmers-revere/opendlv.standard-message-set
            opendlv::proxy::GroundSteeringRequest gsr;
            std::mutex gsrMutex;
            auto onGroundSteeringRequest = [&gsr, &gsrMutex](cluon::data::Envelope &&env){
                std::lock_guard<std::mutex> lck(gsrMutex);
                gsr = cluon::extractMessage<opendlv::proxy::GroundSteeringRequest>(std::move(env));
            };
            od4.dataTrigger(opendlv::proxy::GroundSteeringRequest::ID(), onGroundSteeringRequest);

            //Get AccelerationReading from envelope
            //Using https://github.com/chalmers-revere/opendlv.standard-message-set
            opendlv::proxy::AccelerationReading asr;
            std::mutex asrMutex;
            auto onAccelerationReading = [&asr, &asrMutex](cluon::data::Envelope &&env){
                std::lock_guard<std::mutex> lck(asrMutex);
                asr = cluon::extractMessage<opendlv::proxy::AccelerationReading>(std::move(env));
            };
            od4.dataTrigger(opendlv::proxy::AccelerationReading::ID(), onAccelerationReading); 

            //Get AngularVelocityReading position
            //Using https://github.com/chalmers-revere/opendlv.standard-message-set
            opendlv::proxy::AngularVelocityReading avr;
            std::mutex avrMutex;  
            auto onAngularVelocityReading = [&avr, &avrMutex](cluon::data::Envelope &&env){
                std::lock_guard<std::mutex> lck(avrMutex);
                avr = cluon::extractMessage<opendlv::proxy::AngularVelocityReading>(std::move(env));
            };
            od4.dataTrigger(opendlv::proxy::AngularVelocityReading::ID(), onAngularVelocityReading);         
     
            //Endless loop; end the program by pressing Ctrl-C.
            while (od4.isRunning()) {

                //Increase totalframe count
                totalFrames++;

                //OpenCV data structure to hold an image.
                cv::Mat img;

                //Wait for a notification of a new frame.
                sharedMemory->wait();

                //Lock the shared memory.
                sharedMemory->lock();
                //Query time stamp , needs to be done when memory is locked
                cluon::data::TimeStamp timePoint = sharedMemory->getTimeStamp().second;
                int64_t sampleTime= cluon::time::toMicroseconds(timePoint);
                
                {
                    //Copy the pixels from the shared memory into our own data structure.
                    cv::Mat wrapped(HEIGHT, WIDTH, CV_8UC4, sharedMemory->data());
                    img = wrapped.clone();
                }
                //Unlock the shared memory
                sharedMemory->unlock();
                
                //Get angular velocity from envelope
                angX = avr.angularVelocityX();
                angZ = avr.angularVelocityZ();

                //Get acceleration from envelope
                accX = asr.accelerationX();
                accZ = asr.accelerationZ();

                //Set amount of variables
                Eigen::VectorXd input(5);

                //Create input
                input << angX, angZ, accX, accZ;

                //Estimate steering
                estimatedSteering = input.dot(coefficients);

                //Get steering wheel angle - Used for testing purposes
                correctSteering = gsr.groundSteering();
                     
                /*if(estimatedSteering < -0.03 || estimatedSteering > 0.03) {
                    estimatedSteering = estimatedSteering*2;
                }*/
                apply_doubling_estimation(estimatedSteering);
                
                //Check if estimation is within margins, if not set estimation to value
                /*if(estimatedSteering > 0.230769230769231) {
                    estimatedSteering = 0.230769230769231;
                } else if (estimatedSteering < -0.230769230769231) {
                    estimatedSteering = -0.230769230769231;
                }*/
                apply_min_max(estimatedSteering);
                
                //Calculate steering wheel angle difference - Used for testing purposes
                differenceSteering = (correctSteering - estimatedSteering) / correctSteering;

                //Check if estimation satisfies requirements or not - Used for testing purposes
                if (correctSteering == 0) {
                    if (estimatedSteering >= -0.05 && estimatedSteering <= 0.05) {
                        totalCorrectFrames++;
                    }
                } else {
                    if(differenceSteering >= -0.3 && differenceSteering <= 0.3) {
                        totalCorrectFrames++;
                    }
                }

                //Print sampleTime, estimatedSteering and correctSteering
                //std::cout << "group_14;" << sampleTime << ";" << estimatedSteering << std::endl;

                //Used for testing and presentation purposes
                std::cout << angX << " " << angZ << " " << accX << " " << accZ << " " << correctSteering << std::endl;

                // If you want to access the latest received ground steering, don't forget to lock the mutex:
                {
                    std::lock_guard<std::mutex> lck(gsrMutex);
                }
                
                //get environment variable 
                 char* display_var = std::getenv("DISPLAY");

                // Display image on your screen.
                if (VERBOSE && display_var != nullptr) {
                    cv::imshow(sharedMemory->name().c_str(), img);
                    cv::waitKey(1);
                }
            }
        }
        retCode = 0;
    }
    return retCode;
}
//Load input data
bool load_input_data(const std::string& file_name, Eigen::MatrixXd& input_data) {
    std::ifstream data_file(file_name);

    if (!data_file.is_open()) {
        std::cerr << "Error: Unable to open file " << file_name << std::endl;
        return false;
    }

    std::vector<std::vector<double>> values;
    std::string line;
    while (std::getline(data_file, line)) {
        std::vector<double> row;
        std::istringstream line_stream(line);
        double value;
        while (line_stream >> value) {
            row.push_back(value);
        }
        values.push_back(row);
    }

    if (values.empty()) {
        std::cerr << "Error: The input data file is empty" << std::endl;
        return false;
    }

    int rows = values.size();
    int cols = values[0].size();

    input_data.resize(rows, cols);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            input_data(i, j) = values[i][j];
        }
    }

    return true;
}

//Loads data from file and calculates coefficients
bool load_data(const std::string& file_name, Eigen::MatrixXd& X, Eigen::VectorXd& y) {
    std::ifstream data_file(file_name);
    
    if (!data_file.is_open()) {
        std::cerr << "Error: Unable to open file " << file_name << std::endl;
        return false;
    }

    std::vector<std::vector<double>> values;
    std::string line;
    while (std::getline(data_file, line)) {
        std::vector<double> row;
        std::istringstream line_stream(line);
        double value;
        while (line_stream >> value) {
            row.push_back(value);
        }
        values.push_back(row);
    }

    if (values.empty()) {
        std::cerr << "Error: The data file is empty" << std::endl;
        return false;
    }

    int rows = values.size();
    int cols = values[0].size() - 1; // Subtract 1 for the target variable

    X.resize(rows, cols);
    y.resize(rows);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            X(i, j) = values[i][j];
        }
        y(i) = values[i][cols];
    }

    return true;
}

//Estiamtes steering using coefficients and input data
Eigen::VectorXd linear_regression(const Eigen::MatrixXd& X, const Eigen::VectorXd& y) {
    return (X.transpose() * X).ldlt().solve(X.transpose() * y);
}

void save_results(const std::string& file_name, const Eigen::VectorXd& predictions) {
    std::ofstream output_file(file_name);

    if (!output_file.is_open()) {
        std::cerr << "Error: Unable to open file " << file_name << " for writing" << std::endl;
        return;
    }

    for (int i = 0; i < predictions.size(); ++i) {
        float estimation = predictions(i);
        apply_doubling_estimation(estimation);
        apply_min_max(estimation);
        output_file << estimation << std::endl;
    }

    output_file.close();
}

void combine_files(const std::string& file_1, const std::string& file_2, const std::string& combined) {
    std::ifstream file1(file_1);
    std::ifstream file2(file_2);
    std::ofstream combinedFile(combined);

    if (!file1.is_open() || !file2.is_open() || !combinedFile.is_open()) {
        std::cout << "Failed to open files." << std::endl;
        return;
    }

    std::string line1, line2;

    // Read line by line from both files and combine them in the combinedFile
    while (!file1.eof() && !file2.eof()) {
        if (std::getline(file1, line1) && std::getline(file2, line2)) {
            // Remove trailing whitespace from line1
            line1.erase(std::find_if(line1.rbegin(), line1.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(), line1.end());

            combinedFile << line1 << ";" << line2;

            // Add a line break if not at the end of the files
            if (!file1.eof() && !file2.eof()) {
                combinedFile << std::endl;
            }
        }
    }

    // Close all the files
    file1.close();
    file2.close();
    combinedFile.close();
}


//Check if estimation is within margins, if not set estimation to value
void apply_min_max(float& estimation) {
    if(estimation > 0.230769230769231) {
        estimation = 0.230769230769231;
    } else if (estimation < -0.230769230769231) {
        estimation = -0.230769230769231;
    }
}

void apply_doubling_estimation(float& estimation) {
    if(estimation < -0.03 || estimation > 0.03) {
        estimation = estimation * 2;
    }
}

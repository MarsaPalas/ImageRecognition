*** This project uses TensorFlow and Maxeler to train a model to recognize images.
The goal is to make the process faster by moving one part of code to the DFE.


* TensorFlow was used for model training.
* It was run on CPU and was written in Python using available tensor libraries. 
* TensorFlow code is given in APP/TensorFlow_src folder.
* In order to run it, you'll need to install TensorFlow (https://www.tensorflow.org/versions/master/get_started/os_setup.html).
* After you have installed TensorFlow engine, run the code with "python ./digit_recognition.py"  
* TensorFlow does next things:
  * trains the model
  * evaluates the model
  * stores the model in a file (for latter use by Maxeler)
  * measures the speed of validation in seconds


* Maxeler code was used for model validation.
* It was run on an FPGA chip and was written in Java and C.
* Maxeler code is given in APP/Maxeler_src/DigitRecognition folder and it is ready to be imported and ran in MaxIDE as Simulation or DFE version.
* Folder APP\Maxeler_src\DigitRecognition\EngineCode\src\files contains data needed for validation process:
  * data used for model (Made by TensorFlow code)
	- biases
	- labels
	- weights
  * data used for validation (a file containg 10000 images, each containing one digit; test data was pulled from http://yann.lecun.com/exdb/mnist/)
	- gray_tests
* Maxeler does next things:
  * evaluates the model using gray_tests data
  * measures the validation speed in seconds 


* Comparing FPGA (Maxeler) and CPU (TensorFlow) validation speed shows next results:   
  * DOC/HybridMaxUser, slide 13/15
	

*** This project uses TensorFlow and Maxeler to train and validate a neural network for image recognition.
The goal is to make image recognition faster by moving one part of code to the DFE.


* TensorFlow was used for training.
  It was run on CPU and was written in Python using available tensor libraries. 
  TensorFlow code is not given here. It can be easily reached from official TensofFlow website.


* Maxeler code was used for validation.
  It was run on an FPGA chip and was written in Java and C.
  Maxeler code is given in an APP directory, and it is ready to be imported and ran in MaxIDE as Simulation or real DFE version.
  Folder APP\DigitRecognition\EngineCode\src\files contains:

  * data used for model (TensorFlow outputs; training data pulled from mnist, http://yann.lecun.com/exdb/mnist/)
	- biases
	- labels
	- weights
  * data used for validation (a file containg 10000 images, each containing one digit; data pulled from the same site)
	- gray_tests

* DigitRecognition\CPUCode contains C code that is ran on the CPU.
  The C code feeds gray_tests data to the DFE. 
  The time needed for DFE to finish is measured and written in the console (speed results are presented here, DOC/HybridMaxUser).
  
	

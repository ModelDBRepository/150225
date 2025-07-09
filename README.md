## STEPS TO BE FOLLOWED:

Software that has been used for developing this cerebellar architecture:
- MATLAB VERSION 2009b
- MATLAB ROBOTIC TOOLBOX from Peter Corke version 6
- MICROSOFT VISUAL STUDIO 2008

1. DOWNLOAD THE MATLAB ROBOTIC TOOLBOX FROM:  
   [http://petercorke.com/Robotics_Toolbox.html](http://petercorke.com/Robotics_Toolbox.html). The code has been developed using an older version of this robotic toolbox. Download Release 6 in pkzip format (2/2007).

2. Add the toolbox and the supplied matlab code to the matlab Path.

3. Compile the recursive newton euler function and locate the resulting .mex file under the matlab path too. Under 32bit windows O.S. we can type on the matlab windows command  
   ```
   >> mex frne.c ne.c vmath.c
   ```

4. Compile the cerebellar architecture  
   ```
   mex . CerebellumIODCNAbstract.cpp SimulinkBlockInterface.cpp
   ```

5. Once all the mex files are available, open the Launchers in matlab. `LaunchCerebellumSimulinkExternalForce.m` & `LaunchCerebellumSimulink.m`

6. Run the launchers.

---

2025-07-09: Converted README to Markdown.
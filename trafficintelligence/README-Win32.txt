To be able to use traffic intelligence on windows, you will first have to fetch the 3rd party libraries.
To do this, go in the folder win32-depends and launch win32-depends-installer.bat. It should fetch the library 
and unarchive them correctly.

Second you will need is the project TrajectoryManagementAndAnalysis available at https://bitbucket.org/trajectories/trajectorymanagementandanalysis



If you want the sln to work, the project must be in the same folder where you have cloned trafficintelligence.
To clone the project, use hg clone https://bitbucket.org/trajectories/trajectorymanagementandanalysis

If you've done everything correctly, you should have

/trafficintelligence
/trafficintelligence/win32-depends/boost/
/trafficintelligence/win32-depends/klt/
/trafficintelligence/win32-depends/opencv/
/trafficintelligence/win32-depends/sqlite/
/trajectorymanagementandanalysis




# Implementation of an adated beeclust algorithm for mona robot
## Implementation details
3 versions :
version 1 is less sensitive (robots will not detect each others at great distances) and will work with a robot acting as the source (with light_emitter)
Version 2 is more sensitive and aggregates quicker generally.
It also is confirmed to work with a standard lamp acting as an IR source.
A few things to note :
- It is technically possible for a robot to wrongfully detect another robot as the source in the second algorithm, thought it did not happen in our testing
- The robot is supposed to wait longer (led are red in that case) the more robots it detected. (in the second algorithm).
	However, right now it only takes into account the robots it first detected, which is suboptimal and makes that feature work fairly badly (most often it will just consider one robot around it regardless)
- Blue state means the robot thinks it saw a source (in the second algo).
	In the subsequent 5 seconds ~ it will monitor its sensors to check whether it really was a source.
	It happens that it will mistake a robot for a source, will turn blue but will later change its mind. This is normal behavior.

Chain:

Slicer/RAD --- Network --- Proxy - Robot/Robot Sim

Slicer Sim
Send/receive commands as it would come from slicer
Requires: Boost with ASIO

Proxy
Forwards commands from Slicer to Robot or RobotSim and back
Requires: Boost with ASIO

Robot
Robot Software.
Requires: Proxy, CISST.

Robot Sim
Responds with dummy status - used for testing without robot.
Requires: Proxy


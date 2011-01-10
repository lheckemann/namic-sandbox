/*=========================================================================

  Program:   Open IGT Link -- Example for Tracker Client Program
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <fstream>
#include <math.h>
#include <cstdlib>

#include "igtlOSUtil.h"
#include "igtlTransformMessage.h"
#include "igtlClientSocket.h"

typedef std::vector< std::vector<float> > PointListType;

void GetRandomTestMatrix(igtl::Matrix4x4& matrix);

void ReadFiducials(const char* filename, PointListType& flist)
{
  std::ifstream fs(filename);
  std::vector<float> pos;
  pos.resize(3);

  if (fs.is_open())
    {
    std::string line;
    while(std::getline(fs,line))
      {
      std::stringstream strs(line);
      std::string cell;
      int i = 0;
      int fpush = 1;
      while(std::getline(strs,cell,','))
        {
        const char* cellstr = cell.c_str();
        if (cellstr[0] == '#') // Comment
          {
          fpush = 0;
          break;
          }
        if (i > 0 && i < 4) // Store 2nd - 4th columns
          {
          pos[i] = atof(cellstr);
          }
        i ++;
        }
      if (fpush)
        {
        flist.push_back(pos);
        }
      }
    fs.close();
    }

  // Print out the fiducial list
  std::cerr << "Total " << flist.size() << " fiducials are imported." << std::endl;
  std::cerr << "=============" << std::endl;
  PointListType::iterator iter;
  int i = 0;
  for (iter = flist.begin(); iter != flist.end(); iter ++)
    {
    std::cerr << "Fiducial #" << i << ":  (" 
              << (*iter)[0] << ", " << (*iter)[1] << ", " << (*iter)[2]
              << ")" << std::endl;
    i ++;
    }
  std::cerr << "=============" << std::endl;
}

#define h00(t) (2*(t)**3 - 3*(t)**2     + 1)
#define h10(t) ((t)**3 - 2*(t)**2 + (t))
#define h01(t) (-2*(t)**3 + 3*(t)**2)
#define h11(t) ((t)**3 -   (t)**2)

int step(int segment, float& t, float& dl, float& remainder,
         std::vector< std::vector<float> > p,
         PointListType& traj, PointListType& fm, PointListType& m)
{
  // Take a step of dl and return the path point and new t, return:
  //   t  = new parametric coordinate after step 
  //   dl = desired world space step size (in mm)
  //   p  = point after step
  //   remainder = if step results in parametic coordinate > 1.0, then
  //     this is the amount of world space not covered by step

  std::vector<float> p0 = traj[traj.size()-1]; // last element in path
  remainder = 0;
  float ratio = 100.0;
  int count = 0;
  float dt = dl; // current guess of parametric stepsize
  float t1 = 0.0;
  while (fabs(1.0 - ratio) > 0.05)
    {
    t1 = t + dt;
    std::vector<float> pguess;
    pguess.resize(3);
    float d[3];
    for (int i = 0; i < 3; i ++)
      {
      pguess[i] = h00(t1)*p[segment][i] + h10(t1)*m[segment][i]
        + h01(t1)*p[segment+1][i] + h11(t1)*m[segment+1][i];
      d[i] = pguess[i] - p0[i]
      }
    // calculate distance
    float dist = sqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);
    float ratio = dl / dist;
    dt *= ratio;
    if (dt < 0.00000001)
      {
      return;
      }
    count += 1;
    if (count > 500)
      {
      t = t1;
      p = pguess;

      return;
      }
    }
  if (t1 > 1.0)
    {
    t1 = 1.0;
    p1 = self.point(segment, t1);
    remainder = numpy.linalg.norm(p1 - pguess);
    pguess = p1;
    return (t1, pguess, remainder);
    }
}


int GenerateTrajectoryPoints(PointListType& flist, PointListType& traj, int ntp)
{
  // flist: fiducial points (serve as control points)
  // traj:  trajectory points interpolating control points
  // ntp:   number of trajectory points between two consecutive fiducial points
  //
  // The following code has been created based on EndoscopyGUI.py.
  // EndoscoppyGUI.py was originally developed by Steve Pieper.

  traj.clear();

  // Calculate number of trajectory points
  //traj.resize(ntp*(flist.size()-1) + flist.size());

  // calculate the tangent vectors
  // - fm is forward difference
  // - m is average of in and out vectors
  // - first tangent is out vector, last is in vector
  // - sets self.m
  PointListType fm;
  PointListType m;

  int n = flist.size();
  fm.resize(n);
  m.resize(n);

  for (int 0 = 1; i < n-1; i ++)
    {
    fm[0] = flist[i+1][0] -flist[i][0];
    fm[1] = flist[i+1][1] -flist[i][1];
    fm[2] = flist[i+1][2] -flist[i][2];
    }
  for (int i = 1; i < n-1; i ++)
    {
    m[0] = (fm[i-1][0] + fm[i][0]) / 2;
    m[1] = (fm[i-1][1] + fm[i][1]) / 2;
    m[2] = (fm[i-1][2] + fm[i][2]) / 2;
    }
  m[0][0] = fm[0][0];
  m[0][1] = fm[0][1];
  m[0][2] = fm[0][2];

  m[n-1][0] = fm[n-2][0];
  m[n-1][1] = fm[n-2][1];
  m[n-1][2] = fm[n-2][2];

  traj.clear();
  traj.push_back(flist[0]);

  // calculate the actual path
  // - take steps of self.dl in world space
  // -- if dl steps into next segment, take a step of size "remainder" in the new segment
  // - put resulting points into self.path
  
  int segment = 0;       // which first point of current segment
  float t = 0.0;         // parametric current parametric increment
  float remainder = 0.0; // how much of dl isn't included in current step
  float dl = 0.5;        // desired world space step size (in mm)

  while (segment < n-1)
    {
    t, p, remainder = step(segment, t, dl, remainder, traj)
      if remainder != 0 or t == 1.:
        segment += 1
        t = 0
        if segment < n-1:
          t, p, remainder = self.step(segment, t, remainder)
      self.path.append(p)
            }

  def point(self,segment,t):
    return (self.h00(t)*self.p[segment] + 
              self.h10(t)*self.m[segment] + 
              self.h01(t)*self.p[segment+1] + 
              self.h11(t)*self.m[segment+1])
  
    
}


int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 5) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port> <filename> <fps>"    << std::endl;
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "    <filename> : Fiducial list (*.fcsv generated in 3D Slicer)" << std::endl;
    std::cerr << "    <fps>      : Frequency (fps) to send coordinate" << std::endl;
    exit(0);
    }

  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);
  char*  filename = argv[3];
  double fps      = atof(argv[4]);
  int    interval = (int) (1000.0 / fps);

  //------------------------------------------------------------
  // Read fiducial list
  PointListType flist;
  ReadFiducials(filename, flist);

  //------------------------------------------------------------
  // Establish Connection

  igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer(hostname, port);

  if (r != 0)
    {
    std::cerr << "Cannot connect to the server." << std::endl;
    exit(0);
    }

  //------------------------------------------------------------
  // Allocate Transform Message Class

  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetDeviceName("Tracker");

  //------------------------------------------------------------
  // loop
  while (1)
    {
    igtl::Matrix4x4 matrix;
    GetRandomTestMatrix(matrix);
    transMsg->SetMatrix(matrix);
    transMsg->Pack();
    socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());
    igtl::Sleep(interval); // wait
    }

  //------------------------------------------------------------
  // Close connection

  socket->CloseSocket();

}


//------------------------------------------------------------
// Function to generate random matrix.
void GetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
  float position[3];
  float orientation[4];

  // random position
  static float phi = 0.0;
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 50.0 * cos(phi);
  phi = phi + 0.2;

  // random orientation
  static float theta = 0.0;
  orientation[0]=0.0;
  orientation[1]=0.6666666666*cos(theta);
  orientation[2]=0.577350269189626;
  orientation[3]=0.6666666666*sin(theta);
  theta = theta + 0.1;

  //igtl::Matrix4x4 matrix;
  igtl::QuaternionToMatrix(orientation, matrix);

  matrix[0][3] = position[0];
  matrix[1][3] = position[1];
  matrix[2][3] = position[2];
  
  igtl::PrintMatrix(matrix);
}

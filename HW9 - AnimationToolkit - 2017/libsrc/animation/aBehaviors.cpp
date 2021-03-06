#include "aBehaviors.h"

#include <math.h>
#include "GL/glew.h"
#include "GL/glut.h"

// Base Behavior
///////////////////////////////////////////////////////////////////////////////
Behavior::Behavior()
{
}

Behavior::Behavior( char* name) 
{
	m_name = name;
	m_pTarget = NULL;
}

Behavior::Behavior( Behavior& orig) 
{
	m_name = orig.m_name;
	m_pTarget = NULL;
}

string& Behavior::GetName() 
{
    return m_name;
}

// Behaviors derived from Behavior
//----------------------------------------------------------------------------//
// Seek behavior
///////////////////////////////////////////////////////////////////////////////
// For the given the actor, return a desired velocity in world coordinates
// Seek returns a maximum velocity towards the target
// m_pTarget contains target world position
// actor.getPosition() returns Agent's world position


Seek::Seek( AJoint* target) 
{
	m_name = "seek";
	m_pTarget = target;

}

Seek::Seek( Seek& orig) 
{
	m_name = "seek";
	m_pTarget = orig.m_pTarget;
}


Seek::~Seek()
{
}

vec3 Seek::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();

	// TODO: add your code here to compute Vdesired

	Vdesired = BehaviorController::gMaxSpeed * (targetPos - actorPos).Normalize();

	return Vdesired;
}


// Flee behavior
///////////////////////////////////////////////////////////////////////////////
// For the given the actor, return a desired velocity in world coordinates
// Flee calculates a a maximum velocity away from the target
// m_pTarget contains target world position
// actor.getPosition() returns Agent's world position

Flee::Flee( AJoint* target) 
{
	m_name = "flee";
	m_pTarget = target;
}

Flee::Flee( Flee& orig) 
{
	m_name = "flee";
	m_pTarget = orig.m_pTarget;
}

Flee::~Flee()
{
}

vec3 Flee::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();

	// TODO: add your code here to compute Vdesired

	Vdesired = BehaviorController::gMaxSpeed * (actorPos - targetPos).Normalize();


	return Vdesired;

}

// Arrival behavior
///////////////////////////////////////////////////////////////////////////////
// Given the actor, return a desired velocity in world coordinates
// Arrival returns a desired velocity vector whose speed is proportional to
// the actors distance from the target
// m_pTarget contains target world position
// actor.getPosition() returns Agent's world position
//  Arrival strength is in BehavioralController::KArrival


Arrival::Arrival( AJoint* target) 
{
	m_name = "arrival";
	m_pTarget = target;
}

Arrival::Arrival( Arrival& orig) 
{
	m_name = "arrival";
	m_pTarget = orig.m_pTarget;
}

Arrival::~Arrival()
{
}

vec3 Arrival::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();

	// TODO: add your code here to compute Vdesired

	Vdesired = BehaviorController::KArrival * (targetPos - actorPos);



	return Vdesired;
}


// Departure behavior
///////////////////////////////////////////////////////////////////////////////
// Given the actor, return a desired velocity in world coordinates
// Arrival returns a desired velocity vector whose speed is proportional to
// 1/(actor distance) from the target
// m_pTarget contains target world position
// actor.getPosition() returns Agent's world position
//  Departure strength is in BehavioralController::KDeparture

Departure::Departure(AJoint* target) 
{
	m_name = "departure";
	m_pTarget = target;
}

Departure::Departure( Departure& orig) 
{
	m_name = "departure";
	m_pTarget = orig.m_pTarget;
}

Departure::~Departure()
{
}

vec3 Departure::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();

	// TODO: add your code here to compute Vdesired


	vec3 errVector = targetPos - actorPos;

	Vdesired = -BehaviorController::KDeparture * 1.0 * errVector / errVector.SqrLength();

	return Vdesired;
}


// Avoid behavior
///////////////////////////////////////////////////////////////////////////////
//  For the given the actor, return a desired velocity in world coordinates
//  If an actor is near an obstacle, avoid adds a normal response velocity to the 
//  the desired velocity vector computed using arrival
//  Agent bounding sphere radius is in BehavioralController::radius
//  Avoidance parameters are  BehavioralController::TAvoid and BehavioralController::KAvoid

Avoid::Avoid(AJoint* target, vector<Obstacle>* obstacles) 
{
	m_name = "avoid";
	m_pTarget = target;
	mObstacles = obstacles;
}

Avoid::Avoid( Avoid& orig) 
{
	m_name = "avoid";
	m_pTarget = orig.m_pTarget;
	mObstacles = orig.mObstacles;
}

Avoid::~Avoid()
{
}

vec3 Avoid::calcDesiredVel( BehaviorController* actor)
{

	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	m_actorPos = actor->getPosition();
	m_actorVel = actor->getVelocity();

	//TODO: add your code here
	vec3 Varrival(0, 0, 0);
	// Step 1. compute initial value for Vdesired = Varrival so agent moves toward target

	Varrival = BehaviorController::KArrival * (m_pTarget->getLocalTranslation() - m_actorPos);


	vec3 Vavoid(0, 0, 0);
	//TODO: add your code here to compute Vavoid 



	// Step 2. compute Lb
	//TODO: add your code here

	double Lb = BehaviorController::TAvoid * m_actorVel.Length();

	// Step 3. find closest obstacle 
	//TODO: add your code here

	double closestDistance = INFINITY;
	double closestObstacleRadius = 0.0;
	vec3 closestObstaclePosition = vec3(0.0);
	vector<Obstacle>& obstacleList = *this->mObstacles;
	for (Obstacle& obs : obstacleList)
	{
		vec3 obsLocation = obs.m_Center.getLocalTranslation();
		double dist = (obsLocation - m_actorPos).Length();
		if (dist < closestDistance)
		{
			closestDistance = dist;
			closestObstacleRadius = obs.m_Radius;
			closestObstaclePosition = obsLocation;
		}
	}

	// Step 4. determine whether agent will collide with closest obstacle (only consider obstacles in front of agent)
	//TODO: add your code here

	float thetaWrtY = 0.5 * M_PI - actor->getOrientation()[_Y];
	float cosT = std::cos(thetaWrtY);
	float sinT = std::sin(thetaWrtY);
	mat3 body2World = mat3(vec3(cosT, 0.0f, sinT), vec3(0.0f, 1.0f, 0.0f), vec3(-sinT, 0.0f, cosT));

	vec3 dWorld = closestObstaclePosition - m_actorPos;
	vec3 dBody = body2World.Inverse() * dWorld;

	bool potentialCollide = false;
	//Intentionally layout X, Y this way
	//Due to the difference of coordinate layout
	double dx = dBody[_Z];
	double dy = -dBody[_X];
	double ro = closestObstacleRadius;
	double rb = BehaviorController::gAgentRadius;
	//Note that we are not using absolute of dx, but dx itself instead
	//For we are not considering obstacles behind it
	if (dx > Lb + ro + rb)
	{
		potentialCollide = false;
	}
	else
	{
		if (abs(dy) > ro + rb)
		{
			potentialCollide = false;
		}
		else
		{
			potentialCollide = true;
		}
	}

	// Step 5.  if potential collision detected, compute Vavoid and set Vdesired = Varrival + Vavoid
	//TODO: add your code here

	// Note that dy actually lies on x axis
	vec3 avoidDir = (body2World * vec3(dy, 0.0, 0.0)).Normalize();

	double magAvoid = BehaviorController::KAvoid * (rb + ro - abs(dy)) / (rb + ro);
	Vavoid = magAvoid * avoidDir;


	Vdesired = Varrival;
	if (potentialCollide)
	{
		Vdesired += Vavoid;
	}

	return Vdesired;
	
}

void Avoid::display( BehaviorController* actor)
{
	//  Draw Debug info
	vec3 angle = actor->getOrientation();
	vec3 vel = actor->getVelocity();
	vec3 dir = vec3(cos(angle[1]), 0, sin(angle[1]));
	vec3 probe = dir * (vel.Length()/BehaviorController::gMaxSpeed)*BehaviorController::TAvoid;
	
	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	glVertex3f(m_actorPos[0], m_actorPos[1], m_actorPos[2]);
	glVertex3f(m_obstaclePos[0], m_obstaclePos[1], m_obstaclePos[2]);
	glColor3f(0, 1, 1);
	glVertex3f(m_actorPos[0], m_actorPos[1], m_actorPos[2]);
	glVertex3f(m_actorPos[0] + probe[0], m_actorPos[1] + probe[1], m_actorPos[2] + probe[2]);
	glEnd();
}


// Wander Behavior
///////////////////////////////////////////////////////////////////////////////
// For the given the actor, return a desired velocity in world coordinates
// Wander returns a desired velocity vector whose direction changes at randomly from frame to frame
// Wander strength is in BehavioralController::KWander

Wander::Wander() 
{
	m_name = "wander";
	m_Wander = vec3(1.0, 0.0, 0.0);
}

Wander::Wander( Wander& orig) 
{
	m_name = "wander";
	m_Wander = orig.m_Wander;
}

Wander::~Wander()
{
}

vec3 Wander::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 actorPos = actor->getPosition();

	// compute Vdesired = Vwander

	// Step. 1 find a random direction
	//TODO: add your code here
  
	int randomNum = rand();
	double randomPhase = randomNum % 2 == 0 ? 0.001* randomNum : -0.001 * randomNum;
	vec3 rNoise = vec3(cos(randomPhase), 0.0, sin(randomPhase));

	// Step2. scale it with a noise factor
	//TODO: add your code here

	rNoise *= BehaviorController::KNoise;

	// Step3. change the current Vwander  to point to a random direction
	//TODO: add your code here

	m_Wander = BehaviorController::KWander * (m_Wander + rNoise).Normalize();


	// Step4. scale the new wander velocity vector and add it to the nominal velocity
	//TODO: add your code here

	Vdesired = actor->getVelocity() + m_Wander;


	return Vdesired;
}


// Alignment behavior
///////////////////////////////////////////////////////////////////////////////
// For the given the actor, return a desired velocity vector in world coordinates
// Alignment returns the average velocity of all active agents in the neighborhood
// agents[i] gives the pointer to the ith agent in the environment
// Alignment parameters are in BehavioralController::RNeighborhood and BehavioralController::KAlign


Alignment::Alignment(AJoint* target, vector<AActor>* agents) 
{
	m_name = "alignment";
	m_pAgentList = agents;
	m_pTarget = target;
}



Alignment::Alignment( Alignment& orig) 
{
	m_name = orig.m_name;
	m_pAgentList = orig.m_pAgentList;
	m_pTarget = orig.m_pTarget;

}

Alignment::~Alignment()
{
}

vec3 Alignment::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();
	vector<AActor>& agentList = *m_pAgentList;
	

	// compute Vdesired 
	
	// Step 1. compute value of Vdesired for fist agent (i.e. m_AgentList[0]) using an arrival behavior so it moves towards the target
	 
	BehaviorController* leader = agentList[0].getBehaviorController(); // first agent is the leader
	//TODO: add your code here

	if (actor == leader)
	{
		Vdesired = BehaviorController::KArrival * (targetPos - actorPos);
		return Vdesired;
	}


	// Step 2. if not first agent compute Valign as usual
	//TODO: add your code here
	
	size_t adjacentAgent = 0;
	vec3 sumVelocity = vec3(1.0f);
	for (AActor &agentInstace : agentList)
	{
		BehaviorController* ptrController = agentInstace.getBehaviorController();
		if (ptrController == actor)
		{
			continue;
		}

		vec3 d = actorPos - ptrController->getPosition();
		double dLength = d.Length();
		if (dLength > BehaviorController::RNeighborhood)
		{
			continue;
		}
		else
		{
			adjacentAgent++;
			sumVelocity += ptrController->getVelocity();
		}
	}
	if (adjacentAgent != 0)
	{
		//Assume all the weight values are equal
		sumVelocity *= 1.0 / adjacentAgent;
	}
	Vdesired = BehaviorController::KAlignment * sumVelocity;
	
	return Vdesired;
}

// Separation behavior
///////////////////////////////////////////////////////////////////////////////
// For the given te actor, return a desired velocity vector in world coordinates
// Separation tries to maintain a constant distance between all agents
// within the neighborhood
// agents[i] gives the pointer to the ith agent in the environment
// Separation settings are in BehavioralController::RNeighborhood and BehavioralController::KSeperate

 

Separation::Separation( AJoint* target,  vector<AActor>* agents) 
{
	m_name = "separation";
	m_AgentList = agents;
	m_pTarget = target;
}

Separation::~Separation()
{
}

Separation::Separation( Separation& orig) 
{
	m_name = "separation";
	m_AgentList = orig.m_AgentList;
	m_pTarget = orig.m_pTarget;
}

vec3 Separation::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();
	vector<AActor>& agentList = *m_AgentList;
	
	// compute Vdesired = Vseparate
	// TODO: add your code here to compute Vdesired 
	
	size_t adjacentAgent = 0;
	for (AActor &agentInstace : agentList)
	{
		BehaviorController* ptrController = agentInstace.getBehaviorController();
		if (ptrController == actor)
		{
			continue;
		}

		vec3 d = actorPos - ptrController->getPosition();
		double dLength = d.Length();
		if (dLength > BehaviorController::RNeighborhood)
		{
			continue;
		}
		else
		{
			adjacentAgent++;
			Vdesired += (1.0 / (dLength*dLength)) * d;
		}
	}
	if (adjacentAgent != 0)
	{
		Vdesired *= 1.0 / adjacentAgent;
	}

	Vdesired *= BehaviorController::KSeparation;

	if (Vdesired.Length() < 5.0)
		Vdesired = 0.0;
	
	return Vdesired;
}


// Cohesion behavior
///////////////////////////////////////////////////////////////////////////////
// For the given actor, return a desired velocity vector in world coordinates
// Cohesion moves actors towards the center of the group of agents in the neighborhood
//  agents[i] gives the pointer to the ith agent in the environment
//  Cohesion parameters are in BehavioralController::RNeighborhood and BehavioralController::KCohesion


Cohesion::Cohesion( vector<AActor>* agents) 
{
	m_name = "cohesion";
	m_AgentList = agents;
}

Cohesion::Cohesion( Cohesion& orig) 
{
	m_name = "cohesion";
	m_AgentList = orig.m_AgentList;
}

Cohesion::~Cohesion()
{
}

vec3 Cohesion::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 actorPos = actor->getPosition();
	vector<AActor>& agentList = *m_AgentList;
	
	// compute Vdesired = Vcohesion
	// TODO: add your code here 

	size_t adjacentAgent = 0;
	vec3 massCenter = vec3(0.0);
	for (AActor &agentInstace : agentList)
	{
		BehaviorController* ptrController = agentInstace.getBehaviorController();
		if (ptrController == actor)
		{
			continue;
		}
		vec3 agentPos = ptrController->getPosition();
		vec3 d = actorPos - agentPos;
		double dLength = d.Length();

		if (dLength > BehaviorController::RNeighborhood)
		{
			continue;
		}
		else
		{
			adjacentAgent++;
			massCenter += agentPos;
		}
	}
	if (adjacentAgent != 0)
	{
		massCenter *= 1.0 / adjacentAgent;
		Vdesired = BehaviorController::KCohesion * (massCenter - actorPos);
	}

	return Vdesired;
}

// Flocking behavior
///////////////////////////////////////////////////////////////////////////////
// For the given actor, return a desired velocity vector  in world coordinates
// Flocking combines separation, cohesion, and alignment behaviors
//  Utilize the Separation, Cohesion and Alignment behaviors to determine the desired velocity vector


Flocking::Flocking( AJoint* target,  vector<AActor>* agents) 
{
	m_name = "flocking";
	m_AgentList = agents;
	m_pTarget = target;
}

Flocking::Flocking( Flocking& orig) 
{
	m_name = "flocking";
	m_AgentList = orig.m_AgentList;
	m_pTarget = orig.m_pTarget;
}

Flocking::~Flocking()
{
}

vec3 Flocking::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 actorPos = actor->getPosition();
	vector<AActor>& agentList = *m_AgentList;

	// compute Vdesired = Vflocking
	// TODO: add your code here 

	// Component $1: Sep
	vec3 Sep = vec3(0.0);
	size_t adjacentAgent = 0;
	for (AActor &agentInstace : agentList)
	{
		BehaviorController* ptrController = agentInstace.getBehaviorController();
		if (ptrController == actor)
		{
			continue;
		}

		vec3 d = actorPos - ptrController->getPosition();
		double dLength = d.Length();
		if (dLength > BehaviorController::RNeighborhood)
		{
			continue;
		}
		else
		{
			adjacentAgent++;
			Sep += (1.0 / (dLength*dLength)) * d;
		}
	}
	if (adjacentAgent != 0)
	{
		Sep *= 1.0 / adjacentAgent;
	}

	Sep *= BehaviorController::KSeparation;

	if (Sep.Length() < 5.0)
		Sep = 0.0;

	// Component $2: Align
	vec3 Align = vec3(0.0);
	BehaviorController* leader = agentList[0].getBehaviorController(); adjacentAgent = 0;

	if (actor == leader)
	{
		vec3 targetPos = m_pTarget->getLocalTranslation();
		Align = BehaviorController::KArrival * (targetPos - actorPos);
	}
	else
	{
		vec3 sumVelocity = vec3(0.0);
		for (AActor &agentInstace : agentList)
		{
			BehaviorController* ptrController = agentInstace.getBehaviorController();
			if (ptrController == actor)
			{
				continue;
			}

			vec3 d = actorPos - ptrController->getPosition();
			double dLength = d.Length();
			if (dLength > BehaviorController::RNeighborhood)
			{
				continue;
			}
			else
			{
				adjacentAgent++;
				sumVelocity += ptrController->getVelocity();
			}
		}
		if (adjacentAgent != 0)
		{
			//Assume all the weight values are equal
			sumVelocity *= 1.0 / adjacentAgent;
		}
		Align = BehaviorController::KAlignment * sumVelocity;
	}
	

	// Component $3: Cohesion
	vec3 Coh = vec3(0.0);
	adjacentAgent = 0;
	vec3 massCenter = vec3(0.0);
	for (AActor &agentInstace : agentList)
	{
		BehaviorController* ptrController = agentInstace.getBehaviorController();
		if (ptrController == actor)
		{
			continue;
		}
		vec3 agentPos = ptrController->getPosition();
		vec3 d = actorPos - agentPos;
		double dLength = d.Length();

		if (dLength > BehaviorController::RNeighborhood)
		{
			continue;
		}
		else
		{
			adjacentAgent++;
			massCenter += agentPos;
		}
	}
	if (adjacentAgent != 0)
	{
		massCenter *= 1.0 / adjacentAgent;
		Coh = BehaviorController::KCohesion * (massCenter - actorPos);
	}


	double csep = 0.8;
	double calign = 0.7;
	double ccoh = 1;

	Vdesired = csep * Sep + calign * Align + ccoh * Coh;
	//Vdesired = Align;

	return Vdesired;
}

//	Leader behavior
///////////////////////////////////////////////////////////////////////////////
// For the given actor, return a desired velocity vector in world coordinates
// If the agent is the leader, move towards the target; otherwise, 
// follow the leader at a set distance behind the leader without getting to close together
//  Utilize Separation and Arrival behaviors to determine the desired velocity vector
//  You need to find the leader, who is always agents[0]

Leader::Leader( AJoint* target, vector<AActor>* agents) 
{
	m_name = "leader";
	m_AgentList = agents;
	m_pTarget = target;
}

Leader::Leader( Leader& orig) 
{
	m_name = "leader";
	m_AgentList = orig.m_AgentList;
	m_pTarget = orig.m_pTarget;
}

Leader::~Leader()
{
}

vec3 Leader::calcDesiredVel( BehaviorController* actor)
{
	
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 actorPos = actor->getPosition();
	vector<AActor>& agentList = *m_AgentList;

	// TODO: compute Vdesired  = Vleader
	// followers should stay directly behind leader at a distance of -200 along the local z-axis

	float CSeparation = 4.0;  float CArrival = 2.0;

	BehaviorController* leader = agentList[0].getBehaviorController(); // first agent is the leader
	mat3 Rmat = leader->getGuide().getLocalRotation();  // is rotattion matrix of lead agent

	// Sep component
	vec3 Sep = vec3(0.0);
	size_t adjacentAgent = 0;
	for (AActor &agentInstace : agentList)
	{
		BehaviorController* ptrController = agentInstace.getBehaviorController();
		if (ptrController == actor)
		{
			continue;
		}

		vec3 d = actorPos - ptrController->getPosition();
		double dLength = d.Length();
		if (dLength > BehaviorController::RNeighborhood)
		{
			continue;
		}
		else
		{
			adjacentAgent++;
			Sep += (1.0 / (dLength*dLength)) * d;
		}
	}
	if (adjacentAgent != 0)
	{
		Sep *= 1.0 / adjacentAgent;
	}

	Sep *= BehaviorController::KSeparation;

	if (Sep.Length() < 5.0)
		Sep = 0.0;

	// Arrival component
	vec3 Arriv = vec3(0.0f);
	vec3 targetPos = m_pTarget->getLocalTranslation();

	if (actor == leader)
	{
		Arriv = BehaviorController::KArrival * (targetPos - actorPos);
	}
	else
	{
		vec3 targetPosLeaderPersp = vec3(0.0, 0.0, -200.0);
		vec3 targetFollowerPos = Rmat * targetPosLeaderPersp;
		targetFollowerPos += leader->getPosition();
		Arriv = BehaviorController::KArrival * (targetFollowerPos - actorPos);
	}

	Vdesired = CArrival * Arriv + CSeparation * Sep;

	return Vdesired;
}

///////////////////////////////////////////////////////////////////////////////

AvoidImproved::AvoidImproved(AJoint * target, vector<Obstacle>* obstacles)
	: Avoid(target, obstacles)
{
}

vec3 AvoidImproved::calcDesiredVel(BehaviorController * actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	m_actorPos = actor->getPosition();
	m_actorVel = actor->getVelocity();

	//TODO: add your code here
	vec3 VSeek(0, 0, 0);
	// Step 1. compute initial value for Vdesired = Varrival so agent moves toward target

	VSeek = 200.0 * (m_pTarget->getLocalTranslation() - m_actorPos).Normalize();


	vec3 Vavoid(0, 0, 0);
	//TODO: add your code here to compute Vavoid 



	// Step 2. compute Lb
	//TODO: add your code here

	double Lb = BehaviorController::TAvoid * m_actorVel.Length();

	// Step 3. find closest obstacle 
	//TODO: add your code here

	double closestDistance = INFINITY;
	double closestObstacleRadius = 0.0;
	vec3 closestObstaclePosition = vec3(0.0);
	vector<Obstacle>& obstacleList = *this->mObstacles;
	for (Obstacle& obs : obstacleList)
	{
		vec3 obsLocation = obs.m_Center.getLocalTranslation();
		double dist = (obsLocation - m_actorPos).Length();
		if (dist < closestDistance)
		{
			closestDistance = dist;
			closestObstacleRadius = obs.m_Radius;
			closestObstaclePosition = obsLocation;
		}
	}

	// Step 4. determine whether agent will collide with closest obstacle (only consider obstacles in front of agent)
	//TODO: add your code here

	float thetaWrtY = 0.5 * M_PI - actor->getOrientation()[_Y];
	float cosT = std::cos(thetaWrtY);
	float sinT = std::sin(thetaWrtY);
	mat3 body2World = mat3(vec3(cosT, 0.0f, sinT), vec3(0.0f, 1.0f, 0.0f), vec3(-sinT, 0.0f, cosT));

	vec3 dWorld = closestObstaclePosition - m_actorPos;
	vec3 dBody = body2World.Inverse() * dWorld;

	bool potentialCollide = false;
	//Intentionally layout X, Y this way
	//Due to the difference of coordinate layout
	double dx = dBody[_Z];
	double dy = -dBody[_X];
	double ro = closestObstacleRadius;
	double rb = BehaviorController::gAgentRadius;
	//Note that we are not using absolute of dx, but dx itself instead
	//For we are not considering obstacles behind it
	if (dx > Lb + ro + rb)
	{
		potentialCollide = false;
	}
	else
	{
		if (abs(dy) > ro + rb)
		{
			potentialCollide = false;
		}
		else
		{
			potentialCollide = true;
		}
	}

	// Step 5.  if potential collision detected, compute Vavoid and set Vdesired = Varrival + Vavoid
	//TODO: add your code here

	// Note that dy actually lies on x axis
	vec3 avoidDir = (body2World * vec3(dy, 0.0, 0.0)).Normalize();

	double magAvoid = BehaviorController::KAvoid * (rb + ro - abs(dy)) / (rb + ro);
	Vavoid = magAvoid * avoidDir;


	Vdesired = VSeek;
	if (potentialCollide)
	{
		Vdesired += Vavoid;
	}

	return Vdesired;
}

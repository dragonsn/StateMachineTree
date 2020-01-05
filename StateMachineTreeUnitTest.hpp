#pragma once

#include "StateMachineTree.h"

class AITest : public SNL::StateMachineTree
{
public:
	AITest() { frame = 0;  }
	enum
	{
		Preparing=1,
		Working,
		Ending
	};

	void Tick()
	{
		frame++;
		BGN_SMT
		{
			BGN_STATE(Preparing)
			{
				timer = 3;
				SMT_PAUSE_IF(--timer)
				report("wait 3 ticks to get here");
				timer = 2;
				SMT_CHECK_POINT
				report("get first check point, wait 2 round");
				if (--timer) break;
				timer = 3;
				SMT_CHECK_POINT
				report("get second check point, wait 3 round");
				if (--timer) break;

				BGN_NEW_STATE
				{
					timer = 2;
					BGN_STATE(1)
					{
						report("do  task1-1 2 times in sub state");
						if (--timer) break;
						//go to the same level state 
						GotoState(2, -1);
					}END_STATE

					BGN_STATE(2)
					{
						report("do  task1-2 10 times in sub state");
						timer++;
						if (timer > 10)
						{
							//goto the top level state
							GotoState(Working, 0);
							break;
						}
					}END_STATE
				}END_STATE
			}END_STATE

			BGN_STATE(Working)
			{
				report("keep one doing task2");
				if (frame > 90)
				{
					GotoState(3, 0);
				}
			}END_STATE

			BGN_STATE(Ending)
			{
				report("doing task3");
			}END_STATE
		}
		END_SMT
	}
	void report(const char * tex)
	{
		std::cout<<"FRAME:" << frame << " " << tex<< std::endl;
		for (int i = 0; i < CurrentLevel; i++)
		{
			std::cout << this->DebugStateStack[i] << " <<<===";
		}
		std::cout << std::endl;
	}
	int timer;
	int frame = 0;
};

void UnitTestSMT()
{
	AITest  ai;
	for (int i = 0; i < 100; i++)
	{
		ai.Tick();
	}
}









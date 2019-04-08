#pragma once
#include <assert.h>

namespace SNL
{
	typedef unsigned short StateDataType;
	const static int MaxStateStackDepth = 32;
	const static StateDataType gvc_bt_invalid_ai_state = (StateDataType)-1;
	/*a hiearchical state machine tree ,support fake coroutine */
	class StateMachineTree
	{
	public:
		StateMachineTree()
		{
			Reset();
		}
		virtual ~StateMachineTree() {}

		StateDataType& CurrentLevelState()
		{
			return this->StateStack[CurrentLevel];
		};

		void GotoState(StateDataType b)
		{
			this->StateStack[CurrentLevel] = b;
			this->StateStack[CurrentLevel+1] = 0;
		};

		void GotoState(StateDataType b, int level)
		{
			if (level < 0)
				level = CurrentLevel + level;
			assert(level < MaxStateStackDepth);
			assert(level >= 0);
			this->StateStack[level] = b;
			this->StateStack[level + 1] = 0;
		};

		void ConfirmState(StateDataType b)
		{
			if (CurrentLevelState() != b)
			{
				GotoState(b);
			}

		};

		StateDataType GetState(int level) const
		{
			if (level < 0)
				level = CurrentLevel + level;
			return this->StateStack[level];
		};

		void StepInState()
		{
			CurrentLevel++;
			assert(CurrentLevel < MaxStateStackDepth);
		};

		void StepOutState()
		{
			CurrentLevel--;
			assert(CurrentLevel >= 0);
		};


		void DebugState(const char* name)
		{
			DebugStateStack[CurrentLevel] = name;
			LastDebugStackDepth = CurrentLevel;
		};

		void SetStackDepth(int i)
		{
			CurrentLevel = i;
		};

		void Reset()
		{
			memset(StateStack, 0, sizeof(StateStack));
			memset(DebugStateStack, 0, sizeof(DebugStateStack));
			CurrentLevel = LastDebugStackDepth = 0;
		}
	protected:
		void Nop() {};

		int CurrentLevel;
		StateDataType StateStack[MaxStateStackDepth];
		const char *  DebugStateStack[MaxStateStackDepth];
		int LastDebugStackDepth;
	};


	
}

class  ScopedStateStackPointer
{
public:
	ScopedStateStackPointer(SNL::StateMachineTree * sm)
	{
		sm->StepInState();
		m_sm = sm;
	}

	~ScopedStateStackPointer()
	{
		m_sm->StepOutState();
	}
protected:
	SNL::StateMachineTree * m_sm;
};

#define	 BGN_SMT 			SetStackDepth(0);     \
							{switch (CurrentLevelState()) { case 0:                

#define END_SMT   			}SetStackDepth(0);}


#define BGN_STATE(state)	}case state:   \
							{			  \
							ConfirmState( state);  \
							DebugState( #state );  \
							{ScopedStateStackPointer pt(this);   \
							switch (CurrentLevelState())  { case 0: 


#define END_STATE			}	break;}}{
							

// fake coroutine yield 
	
#define SMT_POINT(state, condition)  }case state:  \
									{\
										ConfirmState(state);\
										DebugState( #state );  \
										if (condition) break; 
							


#define SMT_POINT2(state, condition)   SMT_POINT(state, condition)
#define SMT_CHECK_POINT			SMT_POINT2(__LINE__,false)
#define	SMT_PAUSE_IF(condition) SMT_POINT2(__LINE__,condition) 

#if DO_SMT_UNIT_TEST
class AITest : public SNL::StateMachineTree
{
public:
	AITest() { frame = 0;  }
	void Tick()
	{
		frame++;
		BGN_SMT
		{
			BGN_STATE(1)
			{
				timer = 3;
				SMT_PAUSE_IF(--timer)
				report("haha been here");
				SMT_CHECK_POINT
				report("down down");
				BGN_STATE(1)
				{
					report("1111 11");
					timer++;
					if (timer > 10)
					{
						GotoState(2, 0);
					}
				}END_STATE
			}END_STATE

			BGN_STATE(2)
			{
				report("22222222");
			}END_STATE

			BGN_STATE(3)
			{
				report("33333333");
			}END_STATE
		}
		END_SMT
	}
	void report(const char * tex)
	{
		std::cout<<"FRAME:" << frame << " " << tex<< std::endl;
	}
	int timer;
	int frame = 0;
};
void unit_test_SMT()
{
	AITest  ai;
	for (int i = 0; i < 100; i++)
	{
		ai.Tick();
	}
}
#endif









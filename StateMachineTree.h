#pragma once

#ifndef SNL_ASSERT
#include <assert.h>
#define SNL_ASSERT assert
#endif

namespace SNL
{
	typedef unsigned short StateDataType;

	/*configurable max depth of the state machine tree*/
	const static int MaxStateStackDepth = 64;

	/*a hiearchical state machine tree implementation,support fake coroutine */
	class StateMachineTree
	{
	public:

		struct  ScopedStateStackPointer
		{
		public:
			ScopedStateStackPointer(StateMachineTree * InStateMachine)
			{
				InStateMachine->StepInState();
				CurrentStateMachine = InStateMachine;
			}

			~ScopedStateStackPointer()
			{
				CurrentStateMachine->StepOutState();
			}
		protected:
			StateMachineTree * CurrentStateMachine;
		};

	public:
		StateMachineTree()
		{
			Reset();
		}

		virtual ~StateMachineTree() {}

		StateDataType& CurrentLevelState()
		{
			return this->StateStack[CurrentLevel];
		}

		void GotoState(StateDataType b)
		{
			this->StateStack[CurrentLevel] = b;
			this->StateStack[CurrentLevel+1] = 0;
		}

		void GotoState(StateDataType b, int level)
		{
			if (level < 0)
			{
				level = CurrentLevel + level;
			}

			SNL_ASSERT(level < MaxStateStackDepth);
			SNL_ASSERT(level >= 0);
			this->StateStack[level] = b;
			this->StateStack[level + 1] = 0;
		}

		void ConfirmState(StateDataType b)
		{
			if (CurrentLevelState() != b)
			{
				GotoState(b);
			}
			MaxStackDepth = CurrentLevel > MaxStackDepth ? CurrentLevel : MaxStackDepth;
		}

		StateDataType GetState(int level) const
		{
			if (level < 0)
			{
				level = CurrentLevel + level;
			}
			return this->StateStack[level];
		}

		void StepInState()
		{
			CurrentLevel++;
			SNL_ASSERT(CurrentLevel < MaxStateStackDepth);
		}

		void StepOutState()
		{
			CurrentLevel--;
			SNL_ASSERT(CurrentLevel >= 0);
		}

		void DebugState(const char* name)
		{
			DebugStateStack[CurrentLevel] = name;
			LastDebugStackDepth = CurrentLevel;
		}

		void SetStackDepth(int i)
		{
			CurrentLevel = i;
			MaxStackDepth = i;
		}

		void Reset()
		{
			memset(StateStack, 0, sizeof(StateStack));
			memset(DebugStateStack, 0, sizeof(DebugStateStack));
			CurrentLevel = LastDebugStackDepth = 0;
		}

	protected:
		void Nop() {};
		int CurrentLevel;
		int MaxStackDepth;
		StateDataType StateStack[MaxStateStackDepth];
		const char *  DebugStateStack[MaxStateStackDepth];
		int LastDebugStackDepth;
	};


	
}

#define SMT_REMINDER_STR(x) #x

#define SMT_MACRO_TO_STRING(x) SMT_REMINDER_STR(x)

#define	BGN_SMT 			SetStackDepth(0);     \
							{switch (CurrentLevelState()) { case 0:                

#define END_SMT   			}}


#define BGN_STATE(state)	}case state:   \
							{			  \
							ConfirmState( state);  \
							DebugState( SMT_MACRO_TO_STRING(state) );  \
							{ScopedStateStackPointer pt(this);   \
							switch (CurrentLevelState())  { case 0: 


#define END_STATE			}	break;}}{
							

#define SMT_POINT(state, condition)  }case state:  \
									{\
										ConfirmState(state);\
										DebugState( #state );  \
										if (condition) break; 
							
#define SMT_POINT2(state, condition)   SMT_POINT(state, condition)

#define SMT_CHECK_POINT			SMT_POINT2(__LINE__,false)

#define	SMT_PAUSE_IF(condition) SMT_POINT2(__LINE__,condition) 

#define BGN_NEW_STATE	BGN_STATE(__LINE__)   











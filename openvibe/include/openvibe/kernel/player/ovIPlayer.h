#ifndef __OpenViBE_Kernel_Player_IPlayer_H__
#define __OpenViBE_Kernel_Player_IPlayer_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IScenario;
		class IConfigurationManager;

		enum EPlayerStatus
		{
			PlayerStatus_Stop,
			PlayerStatus_Pause,
			PlayerStatus_Step,
			PlayerStatus_Play,
			PlayerStatus_Forward,
		};

		enum EPlayerReturnCode
		{
			PlayerReturnCode_Sucess,
			PlayerReturnCode_Failed,
			PlayerReturnCode_BoxInitializationFailed
		};

		/**
		 * \class IPlayer
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-09-26
		 * \brief The main player class
		 * \ingroup Group_Player
		 * \ingroup Group_Kernel
		 *
		 * A player is responsible for the playback of a specific scenario.
		 * This player scenario is provided at initialisation stage and should
		 * not be changed until the player terminates. The player idea of calling
		 * a "play" function forces the use of threads in order to avoid CPU locking.
		 * Thus we prefer the idea of having a "short-time" function that is to be
		 * called repeatedly until the player terminates. This is the role of the
		 * \c IPlayer::loop function, that should be called repeatedly by the outside
		 * application.
		 *
		 * \todo Refactor this base class to propose an interface
		 *       more OpenViBE compliant (use IDs, intialize/unitialize
		 *       pairs etc...)
		 */
		class OV_API IPlayer : public IKernelObject
		{
		public:

			/**
			 * \brief Attaches a scenario to this player
			 * \param rScenarioIdentifier [in] : the scenario identifier to attach to this player.
			 *                                   The scenario itself is found from the scenario manager.
			 * \param pLocalConfigurationTokens [in] : An optional map that contains configuration tokens to add.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setScenario(
				const CIdentifier& rScenarioIdentifier,
				const CNameValuePairList* pLocalConfigurationTokens = NULL) = 0;

			/**
			 * \brief returns a reference to the runtime configuration manager
			 * \return a reference to the runtime configuration manager
			 *
			 * When initialized, a player creates a runtime configuration manager
			 * which will be used for scenario and runtime related specific configuration.
			 * The use of this features includes the loading of a delayed configuration file
			 * for instance. The reference should be valid up to the duration of the player itself.
			 *
			 * \sa OpenViBE::Kernel::IConfigurationManager
			 */
			virtual IConfigurationManager& getRuntimeConfigurationManager(void) const = 0;

			/**
			 * \brief returns a reference to the runtime scenario manager
			 * \return a reference to the runtime scenario manager
			 *
			 * When a scenario is loaded, a copy is made and stored in the 
			 * runtime scenario manager. This function allows getting the 
			 * runtime scenario manager that contains the current executed scenario.
			 *
			 * \sa OpenViBE::Kernel::IScenarioManager
			 */
			virtual IScenarioManager& getRuntimeScenarioManager(void) const = 0;

			/**
			 * \brief returns the cidentifier of the associated runtime scenario
			 * \return the cidentifier of the associated runtime scenario
			 *
			 * When initialized, a player copy the scenario in the runtime configuration manager
			 * this method allows getting the identifier of said copied scenario
			 *
			 * \sa OpenViBE::CIdentifier
			 */
			virtual CIdentifier getRuntimeScenarioIdentifier(void) const = 0;

			/**
			 * \brief Initializes this player
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual EPlayerReturnCode initialize(void) = 0;
			/**
			 * \brief Uninitializes this player
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool uninitialize(void) = 0;

			/**
			 * \brief Stops player execution
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool stop(void) = 0;
			/**
			 * \brief Pauses player execution
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool pause(void) = 0;
			/**
			 * \brief Executes one more step and pauses
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool step(void) = 0;
			/**
			 * \brief Makes player run normal speed
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool play(void) = 0;
			/**
			 * \brief Makes player run as fast as possible
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool forward(void) = 0;
			/**
			 * \brief Gets current player status
			 * \return current player status
			 */
			virtual EPlayerStatus getStatus(void) const = 0;

			/**
			 * \brief Sets maximum fast forward factor coefficient
			 * \param f64FastForwardFactor : the maximum speed multiplier to be applied
			 *        when playing in PlayerStatus_Forward mode
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \note If a negative value is passed, it is turned back to 0
			 * \note It the fast forward factor is 0, it tells the player to go as fast as possible
			 */
			virtual bool setFastForwardMaximumFactor(const double f64FastForwardFactor) = 0;
			/**
			 * \brief Gets the maximum fast forward factor coefficient
			 * \return The maximum fast forward factor coefficient.
			 */
			virtual double getFastForwardMaximumFactor(void) const = 0;

			/**
			 * \brief Gets an estimate of the actual time ratio spent in the player's loop
			 * \return the amount of time spent in the player's loop (given in percentage)
			 */
			virtual double getCPUUsage() const = 0;

			/**
			 * \brief "short time" function to be called repeatedly by the outstide application
			 * \param ui64ElapsedTime [in] : real elapsed time given in seconds 32:32
			 * \param ui64MaximumTimeToReach [in] : maximum time to reach given in seconds 32:32
			 * \return \e true if the execution went successfully.
			 * \return \e false in case their was a problem or the execution terminated.
			 *
			 * The \e ui64MaximumTimeToReach parameter guarantees that the player does not
			 * run beyond the specified time when ran in fast forward mode. It defaults
			 * to \c uint64_t(-1) which represents the largest OpenViBE time.
			 */
			virtual bool loop(
				const uint64_t ui64ElapsedTime,
				const uint64_t ui64MaximumTimeToReach = uint64_t(-1)) = 0;

			virtual uint64_t getCurrentSimulatedTime(void) const = 0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Player_Player);
		};
	};
};

#endif // __OpenViBE_Kernel_Player_IPlayer_H__

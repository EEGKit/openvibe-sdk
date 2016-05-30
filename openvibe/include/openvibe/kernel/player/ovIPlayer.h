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
			PlayerStatus_Uninitialized,
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
		class OV_API IPlayer : public OpenViBE::Kernel::IKernelObject
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
			virtual OpenViBE::boolean setScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier,
				const OpenViBE::CNameValuePairList* pLocalConfigurationTokens=NULL)=0;

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
			virtual OpenViBE::Kernel::IConfigurationManager& getRuntimeConfigurationManager(void)=0;

			/**
			 * \brief Initializes this player
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual EPlayerReturnCode initialize(void)=0;
			/**
			 * \brief Uninitializes this player
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean uninitialize(void)=0;

			/**
			 * \brief Stops player execution
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean stop(void)=0;
			/**
			 * \brief Pauses player execution
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean pause(void)=0;
			/**
			 * \brief Executes one more step and pauses
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean step(void)=0;
			/**
			 * \brief Makes player run normal speed
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean play(void)=0;
			/**
			 * \brief Makes player run as fast as possible
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean forward(void)=0;
			/**
			 * \brief Gets current player status
			 * \return current player status
			 */
			virtual OpenViBE::Kernel::EPlayerStatus getStatus(void) const=0;

			/**
			 * \brief Sets maximum fast forward factor coefficient
			 * \param f64FastForwardFactor : the maximum speed multiplier to be applied
			 *        when playing in PlayerStatus_Forward mode
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \note If a negative value is passed, it is turned back to 0
			 * \note It the fast forward factor is 0, it tells the player to go as fast as possible
			 */
			virtual OpenViBE::boolean setFastForwardMaximumFactor(const OpenViBE::float64 f64FastForwardFactor)=0;
			/**
			 * \brief Gets the maximum fast forward factor coefficient
			 * \return The maximum fast forward factor coefficient.
			 */
			virtual OpenViBE::float64 getFastForwardMaximumFactor(void) const=0;

			/**
			 * \brief Gets an estimate of the actual time ratio spent in the player's loop
			 * \return the amount of time spent in the player's loop (given in percentage)
			 */
			virtual OpenViBE::float64 getCPUUsage() const=0;

			/**
			 * \brief "short time" function to be called repeatedly by the outstide application
			 * \param ui64ElapsedTime [in] : real elapsed time given in seconds 32:32
			 * \param ui64MaximumTimeToReach [in] : maximum time to reach given in seconds 32:32
			 * \return \e true if the execution went successfully.
			 * \return \e false in case their was a problem or the execution terminated.
			 *
			 * The \e ui64MaximumTimeToReach parameter guarantees that the player does not
			 * run beyond the specified time when ran in fast forward mode. It defaults
			 * to \c uint64(-1) which represents the largest OpenViBE time.
			 */
			virtual OpenViBE::boolean loop(
				const OpenViBE::uint64 ui64ElapsedTime,
				const OpenViBE::uint64 ui64MaximumTimeToReach=OpenViBE::uint64(-1))=0;

			virtual OpenViBE::uint64 getCurrentSimulatedTime(void) const=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Player_Player);
		};
	};
};

#endif // __OpenViBE_Kernel_Player_IPlayer_H__

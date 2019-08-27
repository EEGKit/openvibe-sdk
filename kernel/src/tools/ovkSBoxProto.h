#pragma once

#include <openvibe/kernel/scenario/ovIBoxProto.h>
#include <system/ovCMemory.h>

namespace
{
	using namespace OpenViBE;
	using namespace Kernel;

	struct SBoxProto : IBoxProto
	{
		SBoxProto(ITypeManager& typeManager) : m_TypeManager(typeManager) { }

		bool addInput(const CString& /*name*/, const CIdentifier& typeId, const CIdentifier& id, const bool /*notify*/) override
		{
			uint64_t v = typeId.toUInteger();
			swap_byte(v, m_ui64InputCountHash);
			swap_byte(m_ui64InputCountHash, 0x7936A0F3BD12D936LL);
			m_oHash = m_oHash.toUInteger() ^ v;
			if (id != OV_UndefinedIdentifier)
			{
				v = id.toUInteger();
				swap_byte(v, 0x2BD1D158F340014D);
				m_oHash = m_oHash.toUInteger() ^ v;
			}
			return true;
		}

		bool addOutput(const CString& /*name*/, const CIdentifier& typeId, const CIdentifier& id, const bool /*notify*/) override
		{
			uint64_t v = typeId.toUInteger();
			swap_byte(v, m_ui64OutputCountHash);
			swap_byte(m_ui64OutputCountHash, 0xCBB66A5B893AA4E9LL);
			m_oHash = m_oHash.toUInteger() ^ v;
			if (id != OV_UndefinedIdentifier)
			{
				v = id.toUInteger();
				swap_byte(v, 0x87CA0F5EFC4FAC68);
				m_oHash = m_oHash.toUInteger() ^ v;
			}
			return true;
		}

		bool addSetting(const CString& /*name*/, const CIdentifier& typeId, const CString& /*defaultValue*/, const bool /*modifiable*/, const CIdentifier& id, const bool /*notify*/) override
		{
			uint64_t v = typeId.toUInteger();
			swap_byte(v, m_ui64SettingCountHash);
			swap_byte(m_ui64SettingCountHash, 0x3C87F3AAE9F8303BLL);
			m_oHash = m_oHash.toUInteger() ^ v;
			if (id != OV_UndefinedIdentifier)
			{
				v = id.toUInteger();
				swap_byte(v, 0x17185F7CDA63A9FA);
				m_oHash = m_oHash.toUInteger() ^ v;
			}
			return true;
		}

		bool addInputSupport(const CIdentifier& typeId) override
		{
			uint64_t v = typeId.toUInteger();
			swap_byte(v, m_ui64OutputCountHash);
			swap_byte(m_ui64OutputCountHash, 0xCBB66A5B893AA4E9LL);
			m_oHash = m_oHash.toUInteger() ^ v;
			return true;
		}

		bool addInputAndDerivedSupport(const CIdentifier& typeId)
		{
			uint64_t v = typeId.toUInteger();
			swap_byte(v, m_ui64OutputCountHash);
			swap_byte(m_ui64OutputCountHash, 0xCBB66A5B893AA4E9LL);
			m_oHash = m_oHash.toUInteger() ^ v;
			return true;
		}

		bool addOutputSupport(const CIdentifier& typeId) override
		{
			uint64_t v = typeId.toUInteger();
			swap_byte(v, m_ui64OutputCountHash);
			swap_byte(m_ui64OutputCountHash, 0xCBB66A5B893AA4E9LL);
			m_oHash = m_oHash.toUInteger() ^ v;
			return true;
		}

		bool addOutputAndDerivedSupport(const CIdentifier& typeId)
		{
			uint64_t v = typeId.toUInteger();
			swap_byte(v, m_ui64OutputCountHash);
			swap_byte(m_ui64OutputCountHash, 0xCBB66A5B893AA4E9LL);
			m_oHash = m_oHash.toUInteger() ^ v;
			return true;
		}

		bool addFlag(const EBoxFlag flag) override
		{
			switch (flag)
			{
				case BoxFlag_CanAddInput: m_oHash = m_oHash.toUInteger() ^ CIdentifier(0x07507AC8, 0xEB643ACE).toUInteger();
					break;
				case BoxFlag_CanModifyInput: m_oHash = m_oHash.toUInteger() ^ CIdentifier(0x5C985376, 0x8D74CDB8).toUInteger();
					break;
				case BoxFlag_CanAddOutput: m_oHash = m_oHash.toUInteger() ^ CIdentifier(0x58DEA69B, 0x12411365).toUInteger();
					break;
				case BoxFlag_CanModifyOutput: m_oHash = m_oHash.toUInteger() ^ CIdentifier(0x6E162C01, 0xAC979F22).toUInteger();
					break;
				case BoxFlag_CanAddSetting: m_oHash = m_oHash.toUInteger() ^ CIdentifier(0xFA7A50DC, 0x2140C013).toUInteger();
					break;
				case BoxFlag_CanModifySetting: m_oHash = m_oHash.toUInteger() ^ CIdentifier(0x624D7661, 0xD8DDEA0A).toUInteger();
					break;
				case BoxFlag_IsDeprecated: m_bIsDeprecated = true;
					break;
				default:
					return false;
			}
			return true;
		}

		bool addFlag(const CIdentifier& flagId) override
		{
			const uint64_t flagValue = m_TypeManager.getEnumerationEntryValueFromName(OV_TypeId_BoxAlgorithmFlag, flagId.toString());
			if (flagValue == OV_UndefinedIdentifier) { return false; }
			// Flags do not modify internal hash
			//m_oHash=m_oHash.toUInteger() ^ flagId.toUInteger();
			return true;
		}

		void swap_byte(uint64_t& v, const uint64_t s)
		{
			uint8_t V[sizeof(v)];
			uint8_t S[sizeof(s)];
			System::Memory::hostToLittleEndian(v, V);
			System::Memory::hostToLittleEndian(s, S);
			for (uint32_t i = 0; i < sizeof(s); i += 2)
			{
				uint32_t j = S[i] % sizeof(v);
				uint32_t k = S[i + 1] % sizeof(v);
				uint8_t t  = V[j];
				V[j]       = V[k];
				V[k]       = t;
			}
			System::Memory::littleEndianToHost(V, &v);
		}

		_IsDerivedFromClass_Final_(IBoxProto, OV_UndefinedIdentifier)

		CIdentifier m_oHash             = OV_UndefinedIdentifier;
		bool m_bIsDeprecated            = false;
		uint64_t m_ui64InputCountHash   = 0x64AC3CB54A35888CLL;
		uint64_t m_ui64OutputCountHash  = 0x21E0FAAFE5CAF1E1LL;
		uint64_t m_ui64SettingCountHash = 0x6BDFB15B54B09F63LL;
		ITypeManager& m_TypeManager;
	};
} // namespace

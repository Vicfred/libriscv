#pragma once

namespace riscv
{
	struct RISCV
	{
		static const uint32_t REG_ZERO = 0;
		static const uint32_t REG_RA   = 1;
		static const uint32_t REG_SP   = 2;
		static const uint32_t REG_GP   = 3;
		static const uint32_t REG_TP   = 4;
		static const uint32_t REG_RETVAL = 10;
		static const uint32_t REG_ARG0   = 10;
		static const uint32_t REG_ARG1   = 11;
		static const uint32_t REG_ARG2   = 12;
		static const uint32_t REG_ARG3   = 13;
		static const uint32_t REG_ARG4   = 14;
		static const uint32_t REG_ARG5   = 15;
		static const uint32_t REG_ARG6   = 16;
		static const uint32_t REG_ARG7   = 17;
		static const uint32_t REG_ECALL  = 17;

		static const char* regname(const uint32_t reg) noexcept
		{
			switch (reg) {
				case 0: return "ZERO";
				case 1: return "RA";
				case 2: return "SP";
				case 3: return "GP";
				case 4: return "TP";
				case 5: return "LR";
				case 6: return "TMP1";
				case 7: return "TMP2";
				case 8: return "SR0";
				case 9: return "SR1";
				case 10: return "A0";
				case 11: return "A1";
				case 12: return "A2";
				case 13: return "A3";
				case 14: return "A4";
				case 15: return "A5";
				case 16: return "A6";
				case 17: return "A7";
				case 18: return "SR2";
				case 19: return "SR3";
				case 20: return "SR4";
				case 21: return "SR5";
				case 22: return "SR6";
				case 23: return "SR7";
				case 24: return "SR8";
				case 25: return "SR9";
				case 26: return "SR10";
				case 27: return "SR11";
				case 28: return "TMP3";
				case 29: return "TMP4";
				case 30: return "TMP5";
				case 31: return "TMP6";
			}
			return "Invalid register";
		}
		static const char* ciname(const uint16_t reg) noexcept
		{
			return regname(reg + 0x8);
		}
	};
}

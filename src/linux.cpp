#include "linux.hpp"
#include "auxvec.hpp"
#include <random>
#include <array>
using namespace riscv;

static inline
void push_arg(Machine<4>& m, std::vector<uint32_t>& vec, uint32_t& dst, const std::string& str)
{
	dst -= str.size();
	dst &= ~0x3; // maintain alignment
	vec.push_back(dst);
	m.copy_to_guest(dst, (const uint8_t*) str.data(), str.size());
}
static inline
void push_aux(Machine<4>& m, std::vector<uint32_t>& vec, AuxVec<uint32_t> aux)
{
	vec.push_back(aux.a_type);
	vec.push_back(aux.a_val);
}
static inline
void push_down(Machine<4>& m, uint32_t& dst, const void* data, size_t size)
{
	dst -= size;
	dst &= ~0x3; // maintain alignment
	m.copy_to_guest(dst, data, size);
}

template <>
void prepare_linux(riscv::Machine<4>& machine,
					const std::vector<std::string>& args,
					const std::vector<std::string>& env)
{
	// start installing at near-end of address space, leaving room on both sides
	// stack below and installation above
	uint32_t dst = machine.cpu.reg(RISCV::REG_SP);

	// inception :)
	auto gen = std::default_random_engine(time(0));
	std::uniform_int_distribution<int> rand(0,256);

	std::array<uint8_t, 16> canary;
	std::generate(canary.begin(), canary.end(), [&] { return rand(gen); });
	push_down(machine, dst, canary.data(), canary.size());
	const uint32_t canary_addr = dst;

	const std::string platform = "RISC-V RV32I";
	push_down(machine, dst, platform.data(), platform.size());
	const uint32_t platform_addr = dst;

	// Arguments to main()
	std::vector<uint32_t> argv;
	argv.push_back(args.size()); // argc
	for (const auto& string : args) {
		push_arg(machine, argv, dst, string);
	}
	argv.push_back(0x0);

	// Environment vars
	for (const auto& string : env) {
		push_arg(machine, argv, dst, string);
	}
	argv.push_back(0x0);

	// Auxiliary vector
	push_aux(machine, argv, {AT_PAGESZ, Page::size()});
	push_aux(machine, argv, {AT_CLKTCK, 100});

	// ELF related
	push_aux(machine, argv, {AT_PHENT, 0});
	push_aux(machine, argv, {AT_PHDR,  0});
	push_aux(machine, argv, {AT_PHNUM, 0});

	// Misc
	push_aux(machine, argv, {AT_BASE, 0});
	push_aux(machine, argv, {AT_FLAGS, 0});
	push_aux(machine, argv, {AT_ENTRY, machine.memory.start_address()});
	push_aux(machine, argv, {AT_HWCAP, 0});
	push_aux(machine, argv, {AT_UID, 0});
	push_aux(machine, argv, {AT_EUID, 0});
	push_aux(machine, argv, {AT_GID, 0});
	push_aux(machine, argv, {AT_EGID, 0});
	push_aux(machine, argv, {AT_SECURE, 1}); // indeed ;)

	push_aux(machine, argv, {AT_PLATFORM, platform_addr});

	// supplemental randomness
	push_aux(machine, argv, {AT_RANDOM, canary_addr});
	push_aux(machine, argv, {AT_NULL, 0});

	// from this point on the stack is starting, pointing @ argc
	// install the arg vector
	const size_t argsize = argv.size() * sizeof(argv[0]);
	dst -= argsize;
	dst &= ~0xF; // mandated 16-byte stack alignment
	machine.copy_to_guest(dst, argv.data(), argsize);
	// re-initialize machine stack-pointer
	machine.cpu.reg(RISCV::REG_SP) = dst;

	if (riscv::verbose_machine) {
		printf("* SP = 0x%X  Argument list: %zu bytes\n", dst, argsize);
		printf("* Program end: 0x%X\n", machine.memory.elf_end_vaddr());
	}
}

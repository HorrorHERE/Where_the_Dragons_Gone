#include <libwdg/libwdg.hpp>
#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>
#include <string_view>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <readline/readline.h>
#include <readline/history.h>
#include <libwdg/process.hpp>
#include <libwdg/error.hpp>

namespace {

	std::vector<std::string> split(std::string_view str, char delimiter)
	{
		std::vector<std::string> out{};
		std::stringstream ss {std::string{str}};
		std::string item;

		while (std::getline(ss, item, delimiter))
		{
			out.push_back(item);
		}

		return out;
	}




	bool is_prefix(std::string_view str, std::string_view of)
	{
		if (str.size() > of.size()) return false;
		return std::equal(str.begin(), str.end(), of.begin());
	}


	void resume(pid_t pid)
	{
		if (ptrace(PTRACE_CONT, pid, nullptr, nullptr) < 0)
		{
			std::cerr << "Couldn't continue\n";
			std::exit(-1);
		}
	}


	void wait_on_signal(pid_t pid)
	{
		int wait_status;
		int options = 0;
		if (waitpid(pid, &wait_status, options) < 0)
		{
			std::perror("Waitpid failed");
		}
	}
	
	void print_stop_reason(const wdg::process& process, wdg::stop_reason reason)
	{
		std::cout << "Process " << process.pid() << ' ';
		switch (reason.reason)
		{
			case wdg::process_state::exited:
				std::cout << "exited with status " << static_cast<int>(reason.info);
				break;
			
			case wdg::process_state::terminated:
				std::cout << "terminated with signal " << sigabbrev_np(reason.info);
				break;
			case wdg::process_state::stopped:
				std::cout << "stopped with signal " << sigabbrev_np(reason.info);
				break;
		}
		std::cout << std::endl;
	}


	void handle_command(std::unique_ptr<wdg::process>& process, std::string_view line)
	{
		auto args = split(line, ' ');
		auto command = args[0];

		if (is_prefix(command, "continue") || is_prefix(command, "c")) {
			process->resume();
			auto reason = process->wait_on_signal();
			print_stop_reason(*process, reason);
		}
		else
		{
			std::cerr << "Unknown command: " << command << "\n";
		}
	}


	std::unique_ptr<wdg::process> attach(int argc, const char** argv)
	{
		if (argc == 3 && argv[1] == std::string_view("-p"))
		{
			pid_t pid = std::atoi(argv[2]);
			return wdg::process::attach(pid);
		}

		else
		{
			const char* program_path = argv[1];
			return wdg::process::launch(program_path);
		}
	}


	void main_loop(std::unique_ptr<wdg::process>& process)
	{
		char* line = nullptr;
		while ((line = readline("wdg> ")) != nullptr)
		{
			std::string line_str;
			if (line == std::string_view(""))
			{
				free(line);
				if (history_length > 0)
				{
					line_str = history_list()[history_length - 1]->line;
				}
			}
			else
			{
				line_str = line;
				add_history(line);
				free(line);
			}
			if (!line_str.empty())
			{
				try
				{
					handle_command(process, line_str);
				}
				catch (const wdg::error& err)
				{
					std::cout << err.what() << '\n';
				}
			}
		}
	}

}


int main(int argc, const char** argv) {
	if (argc == 1) {
		std::cerr << "No arg given\n\nNo dragons here\n";
		return -1;
	}

	try
	{
		auto process = attach(argc, argv);
		main_loop(process);
	}
	catch (const wdg::error& err)
	{
		std::cout << err.what() << "\n";
	}	
	return 0;
}

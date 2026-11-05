#ifndef WDG_PROCESS_HPP
#define WDG_PROCESS_HPP

#include <filesystem>
#include <memory>
#include <sys/types.h>


namespace wdg {

	enum class process_state {
		stopped,
		running,
		exited,
		terminated
	};

	class process {
		public:
			static std::unique_ptr<process> launch(std::filesystem::path path);
			static std::unique_ptr<process> attach(pid_t pid);

			void resume();
			int wait_on_signal();

			pid_t pid() const { return pid_; }

			~process();
		private:

			process(pid_t pid, bool terminate_on_end);			
			process() = delete;
			process(const process&) = delete;
			process& operator = (const process&) = delete;
			pid_t pid_ = 0;
	
	};
}

#endif

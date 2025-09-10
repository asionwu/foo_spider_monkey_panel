#pragma once
#include <events/event.h>

// TODO: add on_size and mouse_move (drag_over as well?) coalescing (https://searchfox.org/mozilla-central/source/dom/ipc/PBrowser.ipdl)
// TODO: add dynamic input task prioritization (kInputLow)

namespace smp
{

class TaskController;
class js_panel_window;

class EventDispatcher
{
public:
	EventDispatcher() = default;
	EventDispatcher(const EventDispatcher&) = delete;
	EventDispatcher& operator=(const EventDispatcher&) = delete;

	static [[nodiscard]] EventDispatcher& Get() noexcept;

public:
	void AddWindow(HWND hWnd, std::shared_ptr<PanelTarget> pTarget) noexcept;
	void RemoveWindow(HWND hWnd) noexcept;

	void NotifyAllAboutExit() noexcept;

public:
	static [[nodiscard]] bool IsRequestEventMessage(UINT msg) noexcept;
	bool ProcessNextEvent(HWND hWnd) noexcept;
	void RequestNextEvent(HWND hWnd) noexcept;
	void OnRequestEventMessageReceived(HWND hWnd) noexcept;

public: // these can be invoked from worker threads
	void PutRunnable(HWND hWnd, std::shared_ptr<Runnable> pRunnable, EventPriority priority = EventPriority::kNormal) noexcept;
	void PutEvent(HWND hWnd, std::unique_ptr<EventBase> pEvent, EventPriority priority = EventPriority::kNormal) noexcept;

	/// @remark Be careful when using this:
	///         - Event must be cloneable.
	///         - Clone operation should not be CPU intensive (e.g. don't copy vectors, but rather wrap it in shared_ptr)
	void PutEventToAll(std::unique_ptr<EventBase> pEvent, EventPriority priority = EventPriority::kNormal) noexcept;

	/// @remark Be careful when using this:
	///         - Event must be cloneable.
	///         - Clone operation should not be CPU intensive (e.g. don't copy vectors, but rather wrap it in shared_ptr)
	void PutEventToOthers(HWND hWnd, std::unique_ptr<EventBase> pEvent, EventPriority priority = EventPriority::kNormal) noexcept;

public:
	// TODO: remove in v2
	/// @remark This is a compatibility hack for window.NotifyOthers()
	void NotifyOthers(HWND hWnd, std::unique_ptr<EventBase> pEvent) noexcept;

private:
	void RequestNextEventImpl(HWND hWnd, TaskController& taskController, std::scoped_lock<std::mutex>& proof) noexcept;

private:
	std::mutex taskControllerMapMutex_;
	std::unordered_map<HWND, std::shared_ptr<TaskController>> taskControllerMap_;

	using IsWaitingForNextMsg = bool;
	std::unordered_map<HWND, IsWaitingForNextMsg> nextEventMsgStatusMap_;
};

} // namespace smp

@page scheduler-programming Scheduler Programming

@section introduction Introduction
Usually when we code in FourKit, everything is ran linearly. Despite this you may wish to schedule some code to be executed at a later point of time.

Do NOT use the built in C# Thread system, it is not safe for FourKit.

@section getting_started Getting Started

To get started we either need the FourKitScheduler instance. You can get this from the server instance.
```csharp
FourKitScheduler scheduler = FourKit.getScheduler();
```

When scheduling a task, you will also need to pass your main plugin class instance. Here is an example of how it can be done:

```csharp
public class ExampleOne
{
	// you can also use a getter
	public static ExampleOne Instance { get; private set; }

	public void OnEnable()
	{
		Instance = this;
	}
}
// then
public class Other
{
	private readonly ExampleOne plugin = ExampleOne.Instance;
}
```

@section scheduling_delayed_task Scheduling a Delayed Task

The scheduling itself is based on ticks, the Minecraft time unit. 1 tick = 0.05 seconds or 1 second = 20 ticks.

Let's say you want to schedule a task to run 30 seconds later which broadcasts a message:

```csharp
FourKitScheduler scheduler = FourKit.getScheduler();

scheduler.runTaskLater(plugin, () => {
  FourKit.broadcastMessage("Mooooo!");
}, 20 * 30 /*<-- the delay */);
```

@section scheduling_repeating_task Scheduling a Repeating Task

Repeating tasks are tasks that can reschedule themselves.

Let's say you want to schedule a task to run 10 seconds later then after that it should repeat itself a finite amount of times with an interval of 5 seconds between each consecutive run:

```csharp
FourKitScheduler scheduler = FourKit.getScheduler();

scheduler.runTaskTimer(plugin, () => {
  FourKit.broadcastMessage("Mooooo!");
}, 20 * 10 /*<-- the initial delay */, 20 * 5 /*<-- the interval */);
```

@section run_task_next_tick Running a Task on the Next Tick

Sometimes we just want to run some code on the next tick:

```csharp
FourKitScheduler scheduler = FourKit.getScheduler();

scheduler.runTask(plugin, () => {
  FourKit.broadcastMessage("Mooooo again!");
});
```

@section canceling_tasks Canceling Tasks

Sometimes we want to just cancel a task!

```csharp
FourKitScheduler scheduler = FourKit.getScheduler();

// Cancel outside
FourKitTask task = scheduler.runTaskLater(plugin, () => {
  FourKit.broadcastMessage("Mooooo again!");
}, 20 * 60);

// Cancel inside
scheduler.runTaskTimer(plugin, () => {
  if (FourKit.getOnlinePlayers().Count == 0) {
    task.cancel(); // <--
    return;
  }
  FourKit.broadcastMessage("Mooooo again!");
}, 0, 20 * 60);

// then
task.cancel(); // <--
```
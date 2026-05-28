using Minecraft.Server.FourKit.Plugin;
using System;
using System.Collections.Generic;
using System.Text;

namespace Minecraft.Server.FourKit.Scheduler
{
    public class FourKitScheduler
    {
        private Dictionary<int, FourKitTask> _taskInstanceMap = new Dictionary<int, FourKitTask>();
        private Dictionary<int, Action> _taskActionMap = new Dictionary<int, Action>();
        //temp task based on about intmax / 3, leaves 715m (715827882) ids for real tasks and 1.4b(1431655765) for temp tasks
        //this is done to avoid needing to cleanup existing task ids, could be handled
        private int _nextTempTaskId = 715827882;
        private int _nextTaskId = 0;

        private int _lastTick = -1;

        /// <summary>
        /// Removes all tasks from the scheduler.
        /// </summary>
        public void cancelAllTasks()
        {
            foreach (var task in _taskInstanceMap.Values)
            {
                NativeBridge.RemoveScheduler?.Invoke(task.getTaskId());
            }

            _taskInstanceMap.Clear();
            _taskActionMap.Clear();

            _nextTempTaskId = 715827882;
            _nextTaskId = 0;
        }

        /// <summary>
        /// Removes task from scheduler.
        /// </summary>
        /// <param name="taskId">Id number of task to be removed.</param>
        public void cancelTask(int taskId)
        {
            if (_taskInstanceMap.ContainsKey(taskId))
            {
                NativeBridge.RemoveScheduler?.Invoke(taskId);
                _taskInstanceMap.Remove(taskId);
                _taskActionMap.Remove(taskId);
            }
        }

        /// <summary>
        /// Removes all tasks associated with a particular plugin from the scheduler.
        /// </summary>
        /// <param name="plugin">Owner of tasks to be removed.</param>
        public void cancelTasks(ServerPlugin plugin)
        {
            List<int> tasksToRemove = new List<int>();

            foreach (var task in _taskInstanceMap.Values)
            {
                if (task.getOwner() == plugin)
                {
                    NativeBridge.RemoveScheduler?.Invoke(task.getTaskId());
                    tasksToRemove.Add(task.getTaskId());
                }
            }

            foreach (var taskId in tasksToRemove)
            {
                _taskInstanceMap.Remove(taskId);
                _taskActionMap.Remove(taskId);
            }
        }

        /// <summary>
        /// Returns a list of all pending tasks. The ordering of the tasks is not related to their order of execution.
        /// </summary>
        /// <returns>Active workers.</returns>
        public List<FourKitTask> getPendingTasks()
        {
            return new List<FourKitTask>(_taskInstanceMap.Values);
        }

        /// <summary>
        /// Check if the task currently running.
        ///
        /// A repeating task might not be running currently, but will be running in the future.
        /// A task that has finished, and does not repeat, will not be running ever again.
        ///
        /// Explicitly, a task is running if there exists a thread for it, and that thread is alive.
        /// </summary>
        /// <param name="taskId">The task to check.</param>
        /// <returns>If the task is currently running.</returns>
        public bool isCurrentlyRunning(int taskId)
        {
            if (_taskInstanceMap.ContainsKey(taskId))
            {
                return _taskInstanceMap[taskId].startedRunning;
            }

            return false;
        }

        /// <summary>
        /// Check if the task queued to be run later.
        ///
        /// If a repeating task is currently running, it might not be queued now but could be in the future.
        /// A task that is not queued, and not running, will not be queued again.
        /// </summary>
        /// <param name="taskId">The task to check.</param>
        /// <returns>If the task is queued to be run.</returns>
        public bool isQueued(int taskId)
        {
            if (_taskInstanceMap.ContainsKey(taskId))
            {
                return !_taskInstanceMap[taskId].startedRunning;
            }

            return false;
        }

        /// <summary>
        /// Returns a task that will run on the next server tick.
        /// </summary>
        /// <param name="plugin">The reference to the plugin scheduling task.</param>
        /// <param name="task">The task to be run.</param>
        /// <returns>A task that contains the id number.</returns>
        /// <exception cref="ArgumentNullException">If plugin is null.</exception>
        /// <exception cref="ArgumentNullException">If task is null.</exception>
        public FourKitTask runTask(ServerPlugin plugin, Action task)
        {
            FourKitTask fourKitTask = new FourKitTask(plugin, _nextTempTaskId++, 0, -1);

            startTask(fourKitTask, task);

            return fourKitTask;

        }

        /// <summary>
        /// Returns a task that will run after the specified number of server ticks.
        /// </summary>
        /// <param name="plugin">The reference to the plugin scheduling task.</param>
        /// <param name="task">The task to be run.</param>
        /// <param name="delay">The ticks to wait before running the task.</param>
        /// <returns>A task that contains the id number.</returns>
        /// <exception cref="ArgumentNullException">If plugin is null.</exception>
        /// <exception cref="ArgumentNullException">If task is null.</exception>
        public FourKitTask runTaskLater(ServerPlugin plugin, Action task, int delay)
        {
            FourKitTask fourKitTask = new FourKitTask(plugin, _nextTempTaskId++, delay, -1);

            startTask(fourKitTask, task);

            return fourKitTask;

        }

        /// <summary>
        /// Returns a task that will repeatedly run until cancelled, starting after the specified number of server ticks.
        /// </summary>
        /// <param name="plugin">The reference to the plugin scheduling task.</param>
        /// <param name="task">The task to be run.</param>
        /// <param name="delay">The ticks to wait before running the task.</param>
        /// <param name="period">The ticks to wait between runs.</param>
        /// <returns>A task that contains the id number.</returns>
        /// <exception cref="ArgumentNullException">If plugin is null.</exception>
        /// <exception cref="ArgumentNullException">If task is null.</exception>
        public FourKitTask runTaskTimer(ServerPlugin plugin, Action task, int delay, int period)
        {
            FourKitTask fourKitTask = new FourKitTask(plugin, _nextTempTaskId++, delay, period);

            startTask(fourKitTask, task);

            return fourKitTask;

        }

        /// <summary>
        /// Starts tracking a task instance and registers it with the native scheduler bridge.
        /// </summary>
        /// <param name="task">The task instance to schedule.</param>
        /// <param name="action">The callback action executed when the task runs.</param>
        internal void startTask(FourKitTask task, Action action)
        {
            task.startedRunning = true;
            _taskInstanceMap[task.getTaskId()] = task;
            _taskActionMap[task.getTaskId()] = action;
            NativeBridge.AddScheduler?.Invoke(task.getTaskId(), task.startDelay, task.runCooldown);
        }

        /// <summary>
        /// Updates scheduled tasks for the current server tick and runs due task callbacks.
        /// </summary>
        /// <param name="currentTick">The current server tick.</param>
        internal void update(int currentTick)
        {
            if (_lastTick == -1) _lastTick = currentTick;
            List<int> tasksToRemove = new List<int>();

            foreach (var task in _taskInstanceMap.Values)
            {
                if (!task.shouldRun)
                {
                    tasksToRemove.Add(task.getTaskId());
                    continue;
                }

                if (task.startDelay > 0)
                {
                    task.startDelay -= (currentTick - _lastTick);

                    if (task.startDelay <= 0)
                    {
                        task.startDelay = 0;
                        _taskActionMap[task.getTaskId()]?.Invoke();
                    }
                    continue;
                }

                if (task.runCooldown == -1)
                {
                    task.lastRunTick = currentTick;
                    _taskActionMap[task.getTaskId()]?.Invoke();
                    tasksToRemove.Add(task.getTaskId());
                }
                else
                {
                    int lastTaskTick = task.lastRunTick;
                    if (lastTaskTick == -1 || (lastTaskTick + task.runCooldown) <= currentTick)
                    {
                        task.lastRunTick = currentTick;
                        _taskActionMap[task.getTaskId()]?.Invoke();
                    }
                }
            }

            foreach (var taskId in tasksToRemove)
            {
                _taskInstanceMap.Remove(taskId);
                _taskActionMap.Remove(taskId);
            }
        }

    }
}

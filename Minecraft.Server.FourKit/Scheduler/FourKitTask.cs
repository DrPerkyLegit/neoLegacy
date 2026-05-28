using Minecraft.Server.FourKit.Plugin;
using System;
using System.Collections.Generic;
using System.Text;

namespace Minecraft.Server.FourKit.Scheduler
{
    public class FourKitTask
    {
        private ServerPlugin owner;
        private int taskId;
        internal bool shouldRun = true;

        internal int startDelay;
        internal int runCooldown;
        internal int lastRunTick = -1;
        internal bool startedRunning = false;

        /// <summary>
        /// Initializes a new task instance with owner, task id, start delay, and run cooldown.
        /// </summary>
        /// <param name="owner">The plugin that owns this task.</param>
        /// <param name="taskId">Task id number.</param>
        /// <param name="startDelay">Delay in server ticks before executing the task.</param>
        /// <param name="runCooldown">Period in server ticks between runs, or -1 for one-shot tasks.</param>
        internal FourKitTask(ServerPlugin owner, int taskId, int startDelay, int runCooldown)
        {
            this.owner = owner;
            this.taskId = taskId;
            this.startDelay = startDelay;
            this.runCooldown = runCooldown;
        }

        /// <summary>
        /// Will attempt to cancel this task.
        /// </summary>
        public void cancel() { shouldRun = false; }

        /// <summary>
        /// Returns the Plugin that owns this task.
        /// </summary>
        /// <returns>The Plugin that owns the task.</returns>
        public ServerPlugin getOwner() { return owner; }

        /// <summary>
        /// Returns the taskId for the task.
        /// </summary>
        /// <returns>Task id number.</returns>
        public int getTaskId() { return taskId; }
   
    }
}

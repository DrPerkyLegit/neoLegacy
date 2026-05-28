using System.Runtime.InteropServices;
using Minecraft.Server.FourKit.Entity;
using Minecraft.Server.FourKit.Event.Inventory;
using Minecraft.Server.FourKit.Inventory;
using Minecraft.Server.FourKit.Scheduler;

namespace Minecraft.Server.FourKit;

public static partial class FourKitHost
{
    internal static PluginLoader? s_loader;
    internal static FourKitScheduler? s_scheduler;

    public static IReadOnlyList<Plugin.ServerPlugin> getLoadedPlugins() => s_loader?.Plugins ?? [];

    [UnmanagedCallersOnly]
    public static void Initialize()
    {
        try
        {
            ServerLog.Info("fourkit", "Initializing scheduler...");
            s_scheduler = new FourKitScheduler();

            ServerLog.Info("fourkit", "Initializing plugin system...");

            // Resolve plugins relative to the host process (Minecraft.Server.exe),
            // NOT the FourKit assembly. AppContext.BaseDirectory points at the
            // "runtime/" subfolder where the self-contained .NET payload lives,
            // so we'd otherwise create plugins inside runtime/plugins/. Use the
            // host exe's directory instead so end users see a top-level plugins/.
            string hostExePath = Environment.ProcessPath ?? AppContext.BaseDirectory;
            string serverRoot = Path.GetDirectoryName(hostExePath) ?? AppContext.BaseDirectory;

            // Redirect AppContext.BaseDirectory to the server root so that
            // plugins using AppContext.BaseDirectory get the exe directory
            // instead of the runtime/ subfolder.
            AppContext.SetData("APP_CONTEXT_BASE_DIRECTORY", serverRoot + Path.DirectorySeparatorChar);

            string pluginsDir = Path.Combine(serverRoot, "plugins");
            s_loader = new PluginLoader();
            s_loader.LoadPlugins(pluginsDir, serverRoot);
            s_loader.EnableAll();

            ServerLog.Info("fourkit", "Plugin system ready.");
        }
        catch (Exception ex)
        {
            ServerLog.Error("fourkit", $"Initialize failed: {ex}");
        }
    }

    internal static FourKitScheduler getScheduler()
    {
        if (s_scheduler == null)
        {
            ServerLog.Warn("fourkit", "Scheduler accessed before initialization. Initializing now...");
            s_scheduler = new FourKitScheduler();
        }

        return s_scheduler;
    }

    [UnmanagedCallersOnly]
    public static void Shutdown()
    {
        try
        {
            ServerLog.Info("fourkit", "Shutting down plugin system...");
            s_loader?.DisableAll();
            s_loader = null;
            ServerLog.Info("fourkit", "Plugin system shut down.");
        }
        catch (Exception ex)
        {
            ServerLog.Error("fourkit", $"Shutdown error: {ex}");
        }
    }


    private static Guid ParseOrHashGuid(string s)
    {
        if (Guid.TryParse(s, out var g)) return g;
        if (string.IsNullOrEmpty(s)) return Guid.NewGuid();
        return new Guid(System.Security.Cryptography.MD5.HashData(System.Text.Encoding.UTF8.GetBytes(s)));
    }

    static double[] s_playerSnapshotBuffer = new double[27];
    static GCHandle? s_playerSnapshotBuffer_Handle = null;

    // double[27] = { x, y, z, health, maxHealth, fallDistance, gameMode, walkSpeed, yaw, pitch, dimension, isSleeping, sleepTimer, sneaking, sprinting, onGround, velocityX, velocityY, velocityZ, allowFlight, sleepingIgnored, experienceLevel, experienceProgress, totalExperience, foodLevel, saturation, exhaustion }
    internal static void SyncPlayerFromNative(Player player)
    {
        if (NativeBridge.GetPlayerSnapshot == null)
            return;

        if (s_playerSnapshotBuffer_Handle == null)
        {
            s_playerSnapshotBuffer_Handle = GCHandle.Alloc(s_playerSnapshotBuffer, GCHandleType.Pinned);
        }

        NativeBridge.GetPlayerSnapshot(player.getEntityId(), s_playerSnapshotBuffer_Handle.GetValueOrDefault().AddrOfPinnedObject());

        int dimId = (int)s_playerSnapshotBuffer[10];
        player.SetDimensionInternal(dimId);
        var world = FourKit.getWorld(dimId);
        player.SetLocation(new Location(world, s_playerSnapshotBuffer[0], s_playerSnapshotBuffer[1], s_playerSnapshotBuffer[2], (float)s_playerSnapshotBuffer[8], (float)s_playerSnapshotBuffer[9]));
        player.SetHealthInternal(s_playerSnapshotBuffer[3]);
        player.SetMaxHealthInternal(s_playerSnapshotBuffer[4]);
        player.SetFallDistanceInternal((float)s_playerSnapshotBuffer[5]);
        player.SetGameModeInternal((GameMode)(int)s_playerSnapshotBuffer[6]);
        player.SetWalkSpeedInternal((float)s_playerSnapshotBuffer[7]);
        player.SetSleepingInternal(s_playerSnapshotBuffer[11] != 0.0);
        player.SetSleepTicksInternal((int)s_playerSnapshotBuffer[12]);
        player.SetSneakingInternal(s_playerSnapshotBuffer[13] != 0.0);
        player.SetSprintingInternal(s_playerSnapshotBuffer[14] != 0.0);
        player.SetOnGroundInternal(s_playerSnapshotBuffer[15] != 0.0);
        player.SetVelocityInternal(s_playerSnapshotBuffer[16], s_playerSnapshotBuffer[17], s_playerSnapshotBuffer[18]);
        player.SetAllowFlightInternal(s_playerSnapshotBuffer[19] != 0.0);
        player.SetSleepingIgnoredInternal(s_playerSnapshotBuffer[20] != 0.0);
        player.SetLevelInternal((int)s_playerSnapshotBuffer[21]);
        player.SetExpInternal((float)s_playerSnapshotBuffer[22]);
        player.SetTotalExperienceInternal((int)s_playerSnapshotBuffer[23]);
        player.SetFoodLevelInternal((int)s_playerSnapshotBuffer[24]);
        player.SetSaturationInternal((float)s_playerSnapshotBuffer[25]);
        player.SetExhaustionInternal((float)s_playerSnapshotBuffer[26]);
    }

    internal static void BroadcastNativeMessage(string message)
    {
        if (string.IsNullOrEmpty(message) || NativeBridge.BroadcastMessage == null)
            return;
        IntPtr ptr = Marshal.StringToCoTaskMemUTF8(message);
        try
        {
            NativeBridge.BroadcastMessage(ptr, System.Text.Encoding.UTF8.GetByteCount(message));
        }
        finally
        {
            Marshal.FreeCoTaskMem(ptr);
        }
    }

    private static void WriteSignOutLens(IntPtr ptr, int[] lens)
    {
        Marshal.Copy(lens, 0, ptr, 4);
    }

    private static string JavaFormat(string format, params string[] args)
    {
        var sb = new System.Text.StringBuilder(format.Length + 64);
        int seqIndex = 0;

        for (int i = 0; i < format.Length; i++)
        {
            char c = format[i];
            if (c == '%' && i + 1 < format.Length)
            {
                char next = format[i + 1];

                if (next == '%')
                {
                    sb.Append('%');
                    i++;
                    continue;
                }

                if (next == 's')
                {
                    if (seqIndex < args.Length)
                        sb.Append(args[seqIndex]);
                    seqIndex++;
                    i++;
                    continue;
                }

                if (char.IsDigit(next))
                {
                    int numStart = i + 1;
                    int j = numStart;
                    while (j < format.Length && char.IsDigit(format[j]))
                        j++;

                    if (j + 1 < format.Length && format[j] == '$' && format[j + 1] == 's')
                    {
                        int argIndex = int.Parse(format.AsSpan(numStart, j - numStart)) - 1;
                        if (argIndex >= 0 && argIndex < args.Length)
                            sb.Append(args[argIndex]);
                        i = j + 1;
                        continue;
                    }
                }

                sb.Append(c);
            }
            else
            {
                sb.Append(c);
            }
        }

        return sb.ToString();
    }


    private static InventoryType MapNativeContainerType(int nativeType)
    {
        return nativeType switch
        {
            0 => InventoryType.CHEST, // CONTAINER
            1 => InventoryType.WORKBENCH, // WORKBENCH
            2 => InventoryType.FURNACE, // FURNACE
            3 => InventoryType.DISPENSER, // TRAP (dispenser)
            4 => InventoryType.ENCHANTING, // ENCHANTMENT
            5 => InventoryType.BREWING, // BREWING_STAND
            6 => InventoryType.MERCHANT, // TRADER_NPC
            7 => InventoryType.BEACON, // BEACON
            8 => InventoryType.ANVIL, // REPAIR_TABLE
            9 => InventoryType.HOPPER, // HOPPER
            10 => InventoryType.DROPPER, // DROPPER
            11 => InventoryType.CHEST, // HORSE
            12 => InventoryType.WORKBENCH, // FIREWORKS
            13 => InventoryType.CHEST, // BONUS_CHEST
            14 => InventoryType.CHEST, // LARGE_CHEST
            15 => InventoryType.ENDER_CHEST,// ENDER_CHEST
            16 => InventoryType.CHEST, // MINECART_CHEST
            17 => InventoryType.HOPPER, // MINECART_HOPPER
            _ => InventoryType.CHEST,
        };
    }

    private static Inventory.Inventory CreateContainerInventory(InventoryType invType, int nativeType, string title, int containerSize, int entityId)
    {
        string name = string.IsNullOrEmpty(title) ? invType.getDefaultTitle() : title;
        int size = containerSize > 0 ? containerSize : invType.getDefaultSize();

        return invType switch
        {
            InventoryType.FURNACE => new Inventory.FurnaceInventory(name, size, entityId),
            InventoryType.BEACON => new Inventory.BeaconInventory(name, size, entityId),
            InventoryType.ENCHANTING => new Inventory.EnchantingInventory(name, size, entityId),
            _ => nativeType switch
            {
                11 => new Inventory.HorseInventory(name, size, entityId), // HORSE
                14 => new Inventory.DoubleChestInventory(name, size, entityId), // LARGE_CHEST
                _ => new Inventory.Inventory(name, invType, size, entityId),
            }
        };
    }

    private static ClickType MapNativeClickType(int nativeClickType, int button)
    {
        return nativeClickType switch
        {
            0 => button == 0 ? ClickType.LEFT : ClickType.RIGHT,
            1 => button == 0 ? ClickType.SHIFT_LEFT : ClickType.SHIFT_RIGHT,
            2 => ClickType.NUMBER_KEY,
            3 => ClickType.MIDDLE,
            4 => button == 1 ? ClickType.CONTROL_DROP : ClickType.DROP,
            5 => ClickType.UNKNOWN,
            6 => ClickType.DOUBLE_CLICK,
            _ => ClickType.UNKNOWN,
        };
    }

    private static InventoryAction DetermineInventoryAction(ClickType click, int slot)
    {
        if (slot == InventoryView.OUTSIDE)
        {
            return click switch
            {
                ClickType.LEFT => InventoryAction.DROP_ALL_CURSOR,
                ClickType.RIGHT => InventoryAction.DROP_ONE_CURSOR,
                ClickType.WINDOW_BORDER_LEFT => InventoryAction.DROP_ALL_CURSOR,
                ClickType.WINDOW_BORDER_RIGHT => InventoryAction.DROP_ONE_CURSOR,
                _ => InventoryAction.NOTHING,
            };
        }

        return click switch
        {
            ClickType.LEFT => InventoryAction.PICKUP_ALL,
            ClickType.RIGHT => InventoryAction.PICKUP_HALF,
            ClickType.SHIFT_LEFT or ClickType.SHIFT_RIGHT => InventoryAction.MOVE_TO_OTHER_INVENTORY,
            ClickType.NUMBER_KEY => InventoryAction.HOTBAR_SWAP,
            ClickType.MIDDLE => InventoryAction.CLONE_STACK,
            ClickType.DROP => InventoryAction.DROP_ONE_SLOT,
            ClickType.CONTROL_DROP => InventoryAction.DROP_ALL_SLOT,
            ClickType.DOUBLE_CLICK => InventoryAction.COLLECT_TO_CURSOR,
            _ => InventoryAction.UNKNOWN,
        };
    }
}

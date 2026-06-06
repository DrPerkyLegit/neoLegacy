using System;
using System.Collections.Generic;
using System.Text;

namespace Minecraft.Server.FourKit.Inventory.NBT;

/// <summary>
/// An NBT tag that stores a float with a 4 byte precision, see <see cref="ItemMeta"/> on how to use it
/// </summary>
public class FloatTag : Tag
{
    public float data;

    public FloatTag(string name, float value) : base(name, Tag.TAG_Float)
    {
        this.data = value;
    }

    /// <inheritdoc/>
    public override bool matches(Tag other)
    {
        if (other is FloatTag otherTag)
        {
            if (data != otherTag.data)
                return false;

            return base.matches(other);
        }

        return false;
    }

    /// <inheritdoc/>
    public override FloatTag clone()
    {
        return new FloatTag(this.name, this.data);
    }
}

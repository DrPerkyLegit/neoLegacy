using System;
using System.Collections.Generic;
using System.Text;

namespace Minecraft.Server.FourKit.Inventory.NBT;

public class FloatTag : Tag
{
    public float data;

    public FloatTag(string name, float value) : base(name, Tag.TAG_Float)
    {
        this.data = value;
    }

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

    public override FloatTag clone()
    {
        return new FloatTag(this.name, this.data);
    }
}

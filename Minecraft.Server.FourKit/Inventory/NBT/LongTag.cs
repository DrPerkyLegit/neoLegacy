using System;
using System.Collections.Generic;
using System.Text;

namespace Minecraft.Server.FourKit.Inventory.NBT;

public class LongTag : Tag
{
    public long data;

    public LongTag(string name, long value) : base(name, Tag.TAG_Long)
    {
        this.data = value;
    }

    public override bool matches(Tag other)
    {
        if (other is LongTag otherTag)
        {
            if (data != otherTag.data)
                return false;

            return base.matches(other);
        }

        return false;
    }

    public override LongTag clone()
    {
        return new LongTag(this.name, this.data);
    }
}

using System;
using System.Collections.Generic;
using System.Text;

namespace Minecraft.Server.FourKit.Inventory.NBT;

public class ShortTag : Tag
{
    public short data;

    public ShortTag(string name, short value) : base(name, Tag.TAG_Short)
    {
        this.data = value;
    }

    public override bool matches(Tag other)
    {
        if (other is ShortTag otherTag)
        {
            if (data != otherTag.data)
                return false;

            return base.matches(other);
        }

        return false;
    }

    public override ShortTag clone()
    {
        return new ShortTag(this.name, this.data);
    }
}

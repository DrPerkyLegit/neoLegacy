using System;
using System.Collections.Generic;
using System.Text;

namespace Minecraft.Server.FourKit.Inventory.NBT;

public class IntTag : Tag
{
    public int data;

    public IntTag(string name, int value) : base(name, Tag.TAG_Int)
    {
        this.data = value;
    }

    public override bool matches(Tag other)
    {
        if (other is IntTag otherTag)
        {
            if (data != otherTag.data)
                return false;

            return base.matches(other);
        }

        return false;
    }

    public override IntTag clone()
    {
        return new IntTag(this.name, this.data);
    }
}

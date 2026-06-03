using System;
using System.Collections.Generic;
using System.Text;

namespace Minecraft.Server.FourKit.Inventory.NBT;

public class DoubleTag : Tag
{
    public double data;

    public DoubleTag(string name, double value) : base(name, Tag.TAG_Double)
    {
        this.data = value;
    }

    public override bool matches(Tag other)
    {
        if (other is DoubleTag otherTag)
        {
            if (data != otherTag.data)
                return false;

            return base.matches(other);
        }

        return false;
    }

    public override DoubleTag clone()
    {
        return new DoubleTag(this.name, this.data);
    }
}

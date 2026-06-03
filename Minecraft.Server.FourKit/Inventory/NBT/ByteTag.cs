using System;
using System.Collections.Generic;
using System.Text;

namespace Minecraft.Server.FourKit.Inventory.NBT;

public class ByteTag : Tag
{
    public byte data;

    public ByteTag(string name, byte value) : base(name, Tag.TAG_Byte)
    {
        this.data = value;
    }

    public override bool matches(Tag other)
    {
        if (other is ByteTag otherTag)
        {
            if (data != otherTag.data)
                return false;

            return base.matches(other);
        }

        return false;
    }

    public override ByteTag clone()
    {
        return new ByteTag(this.name, this.data);
    }
}

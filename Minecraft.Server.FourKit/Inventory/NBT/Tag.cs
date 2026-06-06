using Minecraft.Server.FourKit.Inventory.Meta;

namespace Minecraft.Server.FourKit.Inventory.NBT;

/// <summary>
/// Represents the base type of tag, see <see cref="ItemMeta"/> on how to use it
/// </summary>
public class Tag
{
    internal static readonly byte TAG_INVALID = 99;
    internal static readonly byte TAG_Byte = 1;
    internal static readonly byte TAG_Short = 2;
    internal static readonly byte TAG_Int = 3;
    internal static readonly byte TAG_Long = 4;
    internal static readonly byte TAG_Float = 5;
    internal static readonly byte TAG_Double = 6;
    internal static readonly byte TAG_String = 8;
    internal static readonly byte TAG_List = 9;
    internal static readonly byte TAG_Compound = 10;

    protected string name;
    public readonly byte type;

    internal Tag(string name, byte type)
    {
        this.name = name;
        this.type = type;
    }

    /// <summary>
    /// Gets the name of this NBT tag.
    /// </summary>
    /// <returns>NBT tag name</returns>
    public string getName() => this.name;

    /// <summary>
    /// Sets the name of this NBT tag.
    /// </summary>
    /// <param name="name">The name to set.</param>
    public void setName(string name)
    {
        this.name = name;
    }

    /// <summary>
    /// Check if other tag matches this tags data
    /// </summary>
    /// <returns>true if matching</returns>
    public virtual bool matches(Tag other)
    {
        if (this.name != other.name)
            return false;

        return true;
    }

    /// <summary>
    /// Clones the tag
    /// </summary>
    /// <returns>new tag instance with cloned data</returns>
    public virtual Tag clone()
    {
        return new Tag(this.name, Tag.TAG_INVALID);
    }

    internal static void WriteToBuffer(Tag? tag, byte[] buffer, ref int offset)
    {
        bool isNull = tag == null;

        buffer[offset] = (byte)(isNull ? 1 : 0); offset += 1;

        if (isNull)
            return;

        {
            string tagName = tag.getName();
            byte tagLength = (byte)(tagName.Length & 0xFF);

            buffer[offset] = tagLength; offset += 1;

            for (int i = 0; i < tagLength; i++)
            {
                ushort _castedChar = (ushort)char.ConvertToUtf32(tagName, i);

                buffer[offset] = (byte)((_castedChar >> 8) & 0xFF); offset += 1;
                buffer[offset] = (byte)(_castedChar & 0xFF); offset += 1;
            }
        }

        {
            buffer[offset] = tag.type;
            offset += 1;

            if (tag.type == Tag.TAG_Byte)
            {
                byte value = ((ByteTag)tag).data;

                buffer[offset] = value;
                offset += 1;

            }
            else if (tag.type == Tag.TAG_Short)
            {
                short value = ((ShortTag)tag).data;

                buffer[offset] = (byte)((value >> 8) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)(value & 0xFF);
                offset += 1;
            }
            else if (tag.type == Tag.TAG_Int)
            {
                int value = ((IntTag)tag).data;

                buffer[offset] = (byte)((value >> 24) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 16) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 8) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)(value & 0xFF);
                offset += 1;
            }
            else if (tag.type == Tag.TAG_Long)
            {
                long value = ((LongTag)tag).data;

                buffer[offset] = (byte)((value >> 56) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 48) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 40) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 32) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 24) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 16) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 8) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)(value & 0xFF);
                offset += 1;
            }
            else if (tag.type == Tag.TAG_Float)
            {
                long value = (long)(((FloatTag)tag).data * 32);

                buffer[offset] = (byte)((value >> 56) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 48) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 40) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 32) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 24) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 16) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 8) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)(value & 0xFF);
                offset += 1;
            }
            else if (tag.type == Tag.TAG_Double)
            {
                long value = (long)(((DoubleTag)tag).data * 32);

                buffer[offset] = (byte)((value >> 56) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 48) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 40) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 32) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 24) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 16) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((value >> 8) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)(value & 0xFF);
                offset += 1;
            }
            else if (tag.type == Tag.TAG_String)
            {
                string value = ((StringTag)tag).data;
                short length = (short)(value.Length);

                buffer[offset] = (byte)((length >> 8) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)(length & 0xFF);
                offset += 1;

                for (int i = 0; i < length; i++)
                {
                    ushort _castedChar = (ushort)char.ConvertToUtf32(value, i);

                    buffer[offset] = (byte)((_castedChar >> 8) & 0xFF);
                    offset += 1;
                    buffer[offset] = (byte)(_castedChar & 0xFF);
                    offset += 1;
                }
            }
            else if (tag.type == Tag.TAG_List)
            {
                ListTag listTag = (ListTag)tag;
                short length = (short)(listTag.size());

                buffer[offset] = (byte)((length >> 8) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)(length & 0xFF);
                offset += 1;

                for (int i = 0; i < length; i++)
                {
                    WriteToBuffer(listTag.get(i), buffer, ref offset);
                }
            }
            else if (tag.type == Tag.TAG_Compound)
            {
                List<Tag> allTags = ((CompoundTag)tag).getAllTags();
                short length = (short)(allTags.Count);

                buffer[offset] = (byte)((length >> 8) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)(length & 0xFF);
                offset += 1;

                for (int i = 0; i < length; i++)
                {
                    WriteToBuffer(allTags[i], buffer, ref offset);
                }
            }
        }


    }

    internal static Tag? ReadFromBuffer(byte[] buffer, ref int offset)
    {
        bool isNull = buffer[offset] != 0;
        offset += 1;
        if (isNull)
            return null;

        string tagName = "";
        {
            byte length = buffer[offset];
            offset += 1;

            for (int i = 0; i < length; i++)
            {
                ushort charCode = (ushort)((buffer[offset] >> 8) | buffer[offset + 1]);
                offset += 2;

                tagName += char.ConvertFromUtf32(charCode);
            }
        }

        {
            byte tagType = buffer[offset];
            offset += 1;

            if (tagType == Tag.TAG_Byte)
            {
                byte value = buffer[offset];
                offset += 1;

                return new ByteTag(tagName, value);
            }
            else if (tagType == Tag.TAG_Short)
            {
                short value = (short)((buffer[offset] << 8) | buffer[offset + 1]);
                offset += 2;

                return new ShortTag(tagName, value);
            }
            else if (tagType == Tag.TAG_Int)
            {
                int value = 0;

                value |= buffer[offset + 0];
                value |= buffer[offset + 1] << 8;
                value |= buffer[offset + 2] << 16;
                value |= buffer[offset + 3] << 24;

                offset += 4;

                return new IntTag(tagName, value);
            } else if (tagType == Tag.TAG_Long || tagType == Tag.TAG_Float || tagType == Tag.TAG_Double)
            {
                long value = 0;

                value |= (buffer[offset + 0]);
                value |= (byte)(buffer[offset + 1] << 8);
                value |= (byte)(buffer[offset + 2] << 16);
                value |= (byte)(buffer[offset + 3] << 24);
                value |= (byte)(buffer[offset + 4] << 32);
                value |= (byte)(buffer[offset + 5] << 40);
                value |= (byte)(buffer[offset + 6] << 48);
                value |= (byte)(buffer[offset + 7] << 56);

                offset += 8;

                if (tagType == Tag.TAG_Long)
                {
                    return new LongTag(tagName, value);
                }
                else if (tagType == Tag.TAG_Float)
                {
                    return new FloatTag(tagName, (value / 32));
                }
                else if (tagType == Tag.TAG_Double)
                {
                    return new DoubleTag(tagName, (value / 32));
                }
            } else if (tagType == Tag.TAG_String)
            {
                string value = "";
                short length = (short)((buffer[offset] << 8) | buffer[offset + 1]);
                offset += 2;

                for (int i = 0; i < length; i++)
                {
                    ushort charCode = (ushort)((buffer[offset] << 8) | buffer[offset + 1]);
                    offset += 2;

                    value += char.ConvertFromUtf32(charCode);
                }

                return new StringTag(tagName, value);
            } else if (tagType == Tag.TAG_List)
            {
                ListTag listTag = new ListTag(tagName);
                short length = (short)((buffer[offset] << 8) | buffer[offset + 1]);
                offset += 2;

                for (int i = 0; i < length; i++)
                {
                    Tag? newTag = Tag.ReadFromBuffer(buffer, ref offset);
                    if (newTag != null)
                    {
                        listTag.add(newTag);
                    }
                }

                return listTag;
            }
            else if (tagType == Tag.TAG_Compound)
            {
                CompoundTag compoundTag = new CompoundTag(tagName);
                short length = (short)((buffer[offset] << 8) | buffer[offset + 1]);
                offset += 2;

                for (int i = 0; i < length; i++)
                {
                    Tag? newTag = Tag.ReadFromBuffer(buffer, ref offset);
                    if (newTag != null)
                    {
                        compoundTag.putTag(tagName, newTag);
                    }
                }

                return compoundTag;
            }

        }

        return null;
    }
}
using System;
using System.Collections.Generic;
using System.Text;

namespace Minecraft.Server.FourKit.Inventory.NBT;

/// <summary>
/// An NBT tag that stores a list of tags by index, see <see cref="ItemMeta"/> on how to use it
/// </summary>
public class ListTag : Tag
{
    private List<Tag> _tags = new List<Tag>();

    public ListTag(string name) : base(name, Tag.TAG_List) { }

    /// <summary>
    /// Adds tag to the list
    /// </summary>
    /// <param name="value">The tag to add</param>

    public void add(Tag value) => _tags.Add(value);

    /// <summary>
    /// Gets the tag by index
    /// </summary>
    /// <returns>possible null tag instance</returns>
    public Tag? get(int index) => _tags[index];

    /// <summary>
    /// Gets size of tags inside list
    /// </summary>
    /// <returns>tags count</returns>
    public int size() => _tags.Count;

    /// <summary>
    /// Gets copy of the stored tags list
    /// </summary>
    /// <returns>list of tags</returns>
    public List<Tag> getList() => new List<Tag>(_tags);

    /// <inheritdoc/>
    public override bool matches(Tag other)
    {
        if (other is ListTag otherTag)
        {
            if (_tags.Count != otherTag._tags.Count)
                return false;

            for (int i = 0; i < _tags.Count; i++)
            {
                Tag firstTag = _tags[i];
                Tag secondTag = otherTag._tags[i];

                if (!firstTag.matches(secondTag))
                    return false;

            }

            return base.matches(other);
        }

        return false;
    }

    /// <inheritdoc/>
    public override ListTag clone()
    {
        ListTag newList = new ListTag(this.name);

        foreach (Tag tag in _tags)
        {
            newList.add(tag.clone());
        }

        return newList;
    }
}

using System;
using System.Collections.Generic;
using System.Text;

namespace Minecraft.Server.FourKit.Inventory.NBT;

public class ListTag : Tag
{
    private List<Tag> _tags = new List<Tag>();

    public ListTag(string name) : base(name, Tag.TAG_List) { }

    public void add(Tag value) => _tags.Add(value);
    public Tag get(int index) => _tags[index];

    public int size() => _tags.Count;
    public List<Tag> getList() => new List<Tag>(_tags);

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

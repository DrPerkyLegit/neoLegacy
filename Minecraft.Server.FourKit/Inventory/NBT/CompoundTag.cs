using System;
using System.Collections.Generic;
using System.Text;

namespace Minecraft.Server.FourKit.Inventory.NBT;

public class CompoundTag : Tag
{
    private Dictionary<string, Tag> _tags = new Dictionary<string, Tag>();

    public CompoundTag(string name) : base(name, Tag.TAG_Compound) { }

    public void putTag(string name, Tag value) => _tags[name] = value;
    public void putTag( Tag value) => _tags[value.getName()] = value;
    public Tag getTag(string name) => _tags[name];

    public bool contains(string name) => _tags.ContainsKey(name);

    public int size() => _tags.Count;
    public List<Tag> getAllTags() => new List<Tag>(_tags.Values.ToList());

    public override bool matches(Tag other)
    {
        if (other is CompoundTag otherTag)
        {
            if (_tags.Count != otherTag._tags.Count)
                return false;

            var fistList = _tags.Values.ToList();
            var secondList = otherTag._tags.Values.ToList();

            for (int i = 0; i < _tags.Count; i++)
            {
                Tag firstTag = fistList[i];
                Tag secondTag = secondList[i];

                if (!firstTag.matches(secondTag))
                    return false;

            }

            return base.matches(other);
        }

        return false;
    }

    public override CompoundTag clone()
    {
        CompoundTag newCopound = new CompoundTag(this.name);

        foreach (var entry in _tags)
        {
            newCopound.putTag(entry.Key, entry.Value.clone());
        }

        return newCopound;
    }
}

using System;
using System.Collections.Generic;
using System.Text;

namespace Minecraft.Server.FourKit.Inventory.NBT;

/// <summary>
/// An NBT tag that stores a tag inside a map, see <see cref="ItemMeta"/> on how to use it
/// </summary>
public class CompoundTag : Tag
{
    private Dictionary<string, Tag> _tags = new Dictionary<string, Tag>();

    public CompoundTag(string name) : base(name, Tag.TAG_Compound) { }


    /// <summary>
    /// Adds a tag to the map with the name
    /// </summary>
    /// <param name="name">The name to use.</param>
    /// <param name="value">The tag to insert.</param>
    public void putTag(string name, Tag value) => _tags[name] = value;

    /// <summary>
    /// Adds a tag to the map with the tags name
    /// </summary>
    /// <param name="value">The tag to insert.</param>
    public void putTag(Tag value) => _tags[value.getName()] = value;

    /// <summary>
    /// Gets the tag from with this name
    /// </summary>
    /// <param name="name">The name to use.</param>
    /// <returns>a valid tag if it exists in the map, otherwise null</returns>    
    public Tag? getTag(string name) => (this.contains(name)) ? _tags[name] : null;

    /// <summary>
    /// Checks if the map contains the tag with this name
    /// </summary>
    /// <param name="name">The name to check.</param>
    /// <returns>true of the map contains the tag with this name</returns>    
    public bool contains(string name) => _tags.ContainsKey(name);

    /// <summary>
    /// Gets size of tags inside map
    /// </summary>
    /// <returns>tags count</returns>
    public int size() => _tags.Count;

    /// <summary>
    /// Gets all tags inside the map
    /// </summary>
    /// <returns>list of tags</returns>
    public List<Tag> getAllTags() => new List<Tag>(_tags.Values.ToList());

    /// <inheritdoc/>
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

    /// <inheritdoc/>
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

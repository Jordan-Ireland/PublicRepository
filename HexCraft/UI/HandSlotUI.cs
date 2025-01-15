using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class HandSlotUI : MonoBehaviour
{
    public Image Icon;
    public TextMeshProUGUI Text;

    public void UpdateUI(Sprite icon, int amount)
    {
        if (icon != null)
        {
            Icon.color = new Color(255, 255, 255, 255);
            Icon.sprite = icon;
        }
        else
        {
            Icon.color = new Color(255, 255, 255, 0);
            Icon.sprite = null;
        }
        if (amount > -1)
            Text.SetText(amount.ToString());
        else
            Text.SetText("");
    }
}

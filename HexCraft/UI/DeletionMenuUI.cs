using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DeletionMenuUI : MonoBehaviour, IMenu
{
    public void Close()
    {
        gameObject.SetActive(false);
        Destroy(BuildingCamera.temporaryHexagon);
    }

    public void Open()
    {
        gameObject.SetActive(true);
    }

    public void DeleteHexagon()
    {
        BuildingCamera.DeleteHexagon();
        Close();
    }
}

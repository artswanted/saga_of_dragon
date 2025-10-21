function MutatorInitUI(kWnd, iMutatorNo)
    local bGrayScale = (not mutator_is_enabled(iMutatorNo))
    kWnd:SetGrayScale(bGrayScale)
end

function MutatorChoiceUI(kWnd, iMutatorNo)
    local isEnabled = mutator_is_enabled(iMutatorNo)

    if isEnabled then
        kWnd:SetGrayScale(true)
        mutator_pop(iMutatorNo)
    else
        kWnd:SetGrayScale(false)
        mutator_push(iMutatorNo)
    end
end
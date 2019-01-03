function MMRSetting(props) {
  return (
    <Page>
      <Section
        title={<Text bold align="center">Connections</Text>}>
        <TextInput
          label="MMRClient address"
          placeholder="e.g., http://localhost:27001"
          settingsKey="address"
        />
      </Section>
    </Page>
  );
}

registerSettingsPage(MMRSetting);